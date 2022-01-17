#include "LAppLive2DView.h"
#include <QQuickWindow>
#include "LAppRenderer.h"
#include <QRunnable>
#include "LAppPal.h"
#include <QCoreApplication>
#include "LAppModel.h"
#include "TouchManager.h"
#include "LAppDefine.h"
#include <math.h>
#include "LAppTextureManager.h"
#include "CubismSDK/Framework/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp"

using namespace Csm;
using namespace LAppDefine;

namespace
{
	void FinishedMotion(ACubismMotion* self)
	{
		LAppPal::PrintLog("Motion Finished: %x", self);
	}
}

class CleanupJob : public QRunnable
{
public:
	CleanupJob(LAppRenderer* renderer) : _renderer(renderer) { }
	void run() override { delete _renderer; }
private:
	LAppRenderer* _renderer;
};

LAppLive2DView::LAppLive2DView(QQuickItem* parent)
	: QQuickItem(parent)
	, _t(0)
	, _capture(false)
	, _renderer(nullptr)
	, _cubismOption()
	, _textureManager(nullptr)
{
	_resourcePath = QCoreApplication::applicationDirPath() + QString("/Resources/Haru/Haru.model3.json");
	
	_touchManager = new TouchManager();

	_deviceToScreen = new CubismMatrix44();

	_viewMatrix = new CubismViewMatrix();

	InitializeCubism();

	setAcceptTouchEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);

	connect(this, &QQuickItem::windowChanged, this, &LAppLive2DView::handleWindowChanged);
}

void LAppLive2DView::handleWindowChanged(QQuickWindow* win)
{
	if (win)
	{
		connect(win, &QQuickWindow::sceneGraphInitialized, this, &LAppLive2DView::setupTextures, Qt::DirectConnection);
		connect(win, &QQuickWindow::beforeSynchronizing, this, &LAppLive2DView::sync, Qt::DirectConnection);
		connect(win, &QQuickWindow::sceneGraphInvalidated, this, &LAppLive2DView::cleanup, Qt::DirectConnection);
		win->setColor(Qt::black);
	}
}

void LAppLive2DView::sync()
{
	if (!_renderer)
	{
		_renderer = new LAppRenderer;
		connect(window(), &QQuickWindow::beforeRendering, _renderer, &LAppRenderer::init, Qt::DirectConnection);
		connect(window(), &QQuickWindow::beforeRenderPassRecording, _renderer, &LAppRenderer::paint, Qt::DirectConnection);
	}
	_renderer->setWindow(window());
	_renderer->setViewMatrix(_viewMatrix);
	_renderer->setModel(_model);
}

LAppLive2DView::~LAppLive2DView()
{
	delete _viewMatrix;
	delete _deviceToScreen;
	delete _touchManager;
}

void LAppLive2DView::setT(qreal t)
{
	if (t == _t)
		return;

	_t = t;

	emit tChanged();
	if (window())
		window()->update();
}

void LAppLive2DView::setResourcePath(QString path)
{
	if (path == _resourcePath)
		return;
	if (path.isEmpty() || path.isNull())
		return;
	if (path.endsWith(".model3.json") == false)
		return;

	QStringList stringList = path.split(QString("/"));
	int listLength = stringList.length();
	QString dir = path.section(QString("/"), 0, listLength - 2) + QString("/");
	QString fileName = stringList[listLength - 1].section(QString("."), 0, 0) + QString(".model3.json");
	if (_model->LoadAssets(dir.toStdString().c_str(), fileName.toStdString().c_str()))
		_resourcePath = path;
	else
		return;

	emit resourcePathChanged();
	if (window())
		window()->update();
}

void LAppLive2DView::cleanup()
{
	delete _model;
	_model = nullptr;
	delete _renderer;
	_renderer = nullptr;
	delete _textureManager;
	_textureManager = nullptr;
	CubismFramework::Dispose();
}

void LAppLive2DView::mousePressEvent(QMouseEvent* ev)
{
	if (_renderer == nullptr)
		return;
	if (ev->button() != Qt::LeftButton)
		return;
	_capture = true;
	_touchManager->TouchesBegan(ev->x(), ev->y());
}

void LAppLive2DView::mouseMoveEvent(QMouseEvent* ev)
{
	if (!_capture)
	{
		return;
	}
	if (_renderer == NULL)
	{
		return;
	}

	float screenX = _deviceToScreen->TransformX(_touchManager->GetX());
	float viewX = _viewMatrix->InvertTransformX(screenX);
	float screenY = _deviceToScreen->TransformY(_touchManager->GetY());
	float viewY = _viewMatrix->InvertTransformY(screenY);

	_touchManager->TouchesMoved(ev->x(), ev->y());

	_model->SetDragging(viewX, viewY);
}

void LAppLive2DView::mouseReleaseEvent(QMouseEvent* ev)
{
	if (_renderer == nullptr)
		return;
	if (ev->button() != Qt::LeftButton)
		return;
	if (_capture)
	{
		_capture = false;
		_model->SetDragging(0.0f, 0.0f);
		{
			float x = _deviceToScreen->TransformX(_touchManager->GetX());
			float y = _deviceToScreen->TransformY(_touchManager->GetY());
			if (DebugLogEnable)
			{
				LAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
				LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);
			}
			if (_model->HitTest(HitAreaNameHead, x, y))
			{
				if (DebugLogEnable)
				{
					LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameHead);
				}
				_model->SetRandomExpression();
			}
			else if (_model->HitTest(HitAreaNameBody, x, y))
			{
				if (DebugLogEnable)
				{
					LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameBody);
				}
				_model->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion);
			}
		}
	}
}

void LAppLive2DView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
	QQuickItem::geometryChanged(newGeometry, oldGeometry);
	int w = window()->width();
	int h = window()->height();
	if (w > 0 && h > 0)
	{
		CalculateMatrix();
		if (window() && window()->openglContext())
		{
			window()->openglContext()->functions()->glViewport(0, 0, w, h);
		}
	}
}

void LAppLive2DView::setupTextures()
{
	if (_model == nullptr || _model->IsInitialized() == false)
		return;

	_model->CreateRenderer(window()->openglContext());

	if (_textureManager == nullptr)
		_textureManager = new LAppTextureManager;
	_textureManager->SetWindow(window());
	ICubismModelSetting* modelSetting = _model->GetModelSetting();
	for (csmInt32 modelTextureNumber = 0; modelTextureNumber < modelSetting->GetTextureCount(); modelTextureNumber++)
	{
		// テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
		if (strcmp(modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
		{
			continue;
		}

		//OpenGLのテクスチャユニットにテクスチャをロードする
		QString texturePath = modelSetting->GetTextureFileName(modelTextureNumber);
		int listLength = _resourcePath.split(QString("/")).length();
		QString modelHomeDir = _resourcePath.section(QString("/"), 0, listLength - 2);
		texturePath = modelHomeDir + QString("/") + texturePath;

		LAppTextureManager::TextureInfo* texture = _textureManager->CreateTextureFromPngFile(texturePath.toStdString());
		if (texture == NULL)
			continue;
		const csmInt32 glTextueNumber = texture->id;

		//OpenGL
		_model->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
	}

#ifdef PREMULTIPLIED_ALPHA_ENABLE
	_model->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
	_model->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif
}

void LAppLive2DView::releaseResources()
{
	window()->scheduleRenderJob(new CleanupJob(_renderer), QQuickWindow::BeforeSynchronizingStage);
	_renderer = nullptr;
}

void LAppLive2DView::CalculateMatrix()
{
	int w = window()->width();
	int h = window()->height();

	if (w == 0 || h == 0)
	{
		return;
	}

	float ratio = static_cast<float>(w) / static_cast<float>(h);
	float left = -ratio;
	float right = ratio;
	float bottom = ViewLogicalLeft;
	float top = ViewLogicalRight;

	_viewMatrix->SetScreenRect(left, right, bottom, top);
	_viewMatrix->Scale(ViewScale, ViewScale);

	_deviceToScreen->LoadIdentity();
	if (w > h)
	{
		float screenW = fabsf(right - left);
		_deviceToScreen->ScaleRelative(screenW / w, -screenW / w);
	}
	else
	{
		float screenH = fabsf(top - bottom);
		_deviceToScreen->ScaleRelative(screenH / h, -screenH / h);
	}
	_deviceToScreen->TranslateRelative(-w * 0.5f, -h * 0.5f);

	_viewMatrix->SetMaxScale(ViewMaxScale);
	_viewMatrix->SetMinScale(ViewMinScale);

	_viewMatrix->SetMaxScreenRect(
		ViewLogicalMaxLeft,
		ViewLogicalMaxRight,
		ViewLogicalMaxBottom,
		ViewLogicalMaxTop
	);
}

void LAppLive2DView::InitializeCubism()
{
	_cubismOption.LogFunction = LAppPal::PrintMessage;
	_cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
	CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

	CubismFramework::Initialize();

	_model = new LAppModel;
	QStringList stringList = _resourcePath.split(QString("/"));
	int listLength = stringList.length();
	QString dir = _resourcePath.section(QString("/"), 0, listLength - 2) + QString("/");
	QString fileName = stringList[listLength - 1].section(QString("."), 0, 0) + QString(".model3.json");
	if (_model->LoadAssets(dir.toStdString().c_str(), fileName.toStdString().c_str()) == false)
		LAppPal::PrintLog("Model load error: %s", _resourcePath.toStdString().c_str());

	LAppPal::UpdateTime();
}
