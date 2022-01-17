#include "LAppRenderer.h"
#include <QQuickWindow>
#include "LAppPal.h"
#include "LAppDefine.h"
#include <QOpenGLFunctions>
#include "LAppModel.h"

using namespace LAppDefine;
using namespace Csm;

LAppRenderer::LAppRenderer()
	: _viewMatrix(nullptr)
	, _model(nullptr)
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

void LAppRenderer::paint()
{
	LAppPal::UpdateTime();

	QOpenGLFunctions* f = _window->openglContext()->functions();

	f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
}