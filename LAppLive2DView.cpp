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
#include <QOpenGLFunctions>

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
	, _modelUpdated(true)
	, _renderer(nullptr)
	, _cubismOption()
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
		connect(win, &QQuickWindow::beforeSynchronizing, this, &LAppLive2DView::sync, Qt::DirectConnection);
		connect(win, &QQuickWindow::sceneGraphInvalidated, this, &LAppLive2DView::cleanup, Qt::DirectConnection);
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
	_renderer->setModelUpdated(_modelUpdated);
	_modelUpdated = false;
	_renderer->setResourcePath(_resourcePath);
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
	if (path.startsWith(QStringLiteral("file:///")))
		path = path.section(QStringLiteral("file:///"), -1);
	if (path == _resourcePath)
		return;
	if (path.isEmpty() || path.isNull())
		return;
	if (path.endsWith(".model3.json") == false)
		return;

	_resourcePath = path;
	emit resourcePathChanged();
	_modelUpdated = true;

	if (window())
		window()->update();
}

void LAppLive2DView::handleMousePressEvent(int button, double x, double y)
{
	if (_renderer == nullptr)
		return;
	if (static_cast<Qt::MouseButton>(button) != Qt::LeftButton)
		return;
	_capture = true;
	_touchManager->TouchesBegan(static_cast<float>(x), static_cast<float>(y));
	if (window())
		window()->update();
}

void LAppLive2DView::handleMouseMoveEvent(int, double x, double y)
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

	_touchManager->TouchesMoved(static_cast<float>(x), static_cast<float>(y));

	_renderer->SetDragging(viewX, viewY);
	if (window())
		window()->update();
}

void LAppLive2DView::handleMouseReleaseEvent(int button, double, double)
{
	if (_renderer == nullptr)
		return;
	if (static_cast<Qt::MouseButton>(button) != Qt::LeftButton)
		return;
	if (_capture)
	{
		_capture = false;
		_renderer->SetDragging(0.0f, 0.0f);
		{
			float x = _deviceToScreen->TransformX(_touchManager->GetX());
			float y = _deviceToScreen->TransformY(_touchManager->GetY());
			if (DebugLogEnable)
			{
				LAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
				LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);
			}
			if (_renderer->HitTest(HitAreaNameHead, x, y))
			{
				if (DebugLogEnable)
				{
					LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameHead);
				}
				_renderer->SetRandomExpression();
			}
			else if (_renderer->HitTest(HitAreaNameBody, x, y))
			{
				if (DebugLogEnable)
				{
					LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameBody);
				}
				_renderer->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion);
			}
		}
	}

	if (window())
		window()->update();
}

void LAppLive2DView::cleanup()
{
	delete _renderer;
	_renderer = nullptr;
	CubismFramework::Dispose();
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

	LAppPal::UpdateTime();
}
