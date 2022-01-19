#include "LAppRenderer.h"
#include <QQuickWindow>
#include "LAppPal.h"
#include "LAppDefine.h"
#include <QOpenGLFunctions>
#include "LAppModel.h"
#include "LAppTextureManager.h"
#include "CubismSDK/Framework/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp"

using namespace LAppDefine;
using namespace Csm;

LAppRenderer::LAppRenderer()
	: _viewMatrix(nullptr)
	, _model(nullptr)
	, _modelUpdated(false)
{
}

LAppRenderer::~LAppRenderer()
{
}

void LAppRenderer::setWindow(QQuickWindow* window)
{
	_window = window;
}

void LAppRenderer::setViewMatrix(CubismViewMatrix* viewMat)
{
	_viewMatrix = viewMat;
}

void LAppRenderer::setModel(LAppModel* model)
{
	_model = model;
}

void LAppRenderer::setTextureManager(LAppTextureManager* mgr)
{
	_textureManager = mgr;
}

void LAppRenderer::setModelUpdated(bool updated)
{
	_modelUpdated = updated;
}

void LAppRenderer::setResourcePath(QString path)
{
	_resourcePath = path;
}

void LAppRenderer::paint()
{
	LAppPal::UpdateTime();

	QOpenGLFunctions* f = _window->openglContext()->functions();

	f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	f->glClearDepthf(1.0f);

	int w = _window->width();
	int h = _window->height();

	CubismMatrix44 projection;
	if (_model->IsInitialized() == false)
	{
		LAppPal::PrintLog("Model is not initialized");
		return;
	}
	if (_model->GetModel()->GetCanvasWidth() > 1.0f && w < h)
	{
		// 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
		_model->GetModelMatrix()->SetWidth(2.0f);
		projection.Scale(1.0f, static_cast<float>(w) / static_cast<float>(h));
	}
	else
	{
		projection.Scale(static_cast<float>(h) / static_cast<float>(w), 1.0f);
	}

	// 必要があればここで乗算
	if (_viewMatrix != NULL)
	{
		projection.MultiplyByMatrix(_viewMatrix);
	}

	_model->Update();
	_model->Draw(projection);///< 参照渡しなのでprojectionは変質する
}

void LAppRenderer::init()
{
	QOpenGLFunctions* f = _window->openglContext()->functions();
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	f->glEnable(GL_BLEND);
	f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (_modelUpdated)
	{
		if (_window == nullptr)
			return;
		if (_model == nullptr || _model->IsInitialized() == false)
			return;

		_model->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->Initialize(_model->GetModel());

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
}