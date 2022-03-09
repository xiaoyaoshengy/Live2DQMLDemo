#include "LAppRenderer.h"
#include <QQuickWindow>
#include "LAppPal.h"
#include "LAppDefine.h"
#include <QOpenGLFunctions>
#include "LAppModel.h"
#include "LAppTextureManager.h"
#include "Framework/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp"
#include <QAudioOutput>

using namespace LAppDefine;
using namespace Csm;

LAppRenderer::LAppRenderer()
	: _viewMatrix(nullptr)
	, _model(nullptr)
	, _modelUpdated(false)
	, _textureManager(nullptr)
{
	_audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice().preferredFormat(), this);
	connect(_audioOutput, &QAudioOutput::stateChanged, this, &LAppRenderer::handleVoiceStateChanged);
}

LAppRenderer::~LAppRenderer()
{
	if (_audioOutput)
		delete _audioOutput;
	_audioOutput = nullptr;
}

void LAppRenderer::setWindow(QQuickWindow* window)
{
	_window = window;
}

void LAppRenderer::setViewMatrix(CubismViewMatrix* viewMat)
{
	_viewMatrix = viewMat;
}

void LAppRenderer::setModelUpdated(bool updated)
{
	_modelUpdated = updated;
}

void LAppRenderer::setResourcePath(QString path)
{
	if (path == _resourcePath)
		_modelUpdated = false;
	else
	{
		_oldPath = _resourcePath;
		_resourcePath = path;
	}
}

void LAppRenderer::SetDragging(float px, float py)
{
	if (_model)
		_model->SetDragging(px, py);
}

bool LAppRenderer::HitTest(const char* hitAreaName, float px, float py)
{
	if (_model)
		return _model->HitTest(hitAreaName, px, py);
	return false;
}

void LAppRenderer::SetRandomExpression()
{
	if (_model)
		_model->SetRandomExpression();
}

void LAppRenderer::StartRandomMotion(const char* group, int priority, ACubismMotion::FinishedMotionCallback onFinishedMotionCallback)
{
	_model->StartRandomMotion(group, priority, onFinishedMotionCallback);
}

void LAppRenderer::PlayVoice(QString voiceFilePath)
{
	if (_audioFile.isOpen())
		_audioFile.close();
	_audioFile.setFileName(voiceFilePath);
	if (!_audioFile.open(QIODevice::ReadOnly))
	{
		LAppPal::PrintLog("sound file open error: %s", voiceFilePath.toStdString().c_str());
	}
	else
	{
		_audioOutput->stop();
		_audioOutput->start(&_audioFile);
	}
}

void LAppRenderer::paint()
{
	if (_window == nullptr || _model == nullptr)
		return;

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

		if (_model)
		{
			delete _model;
		}
		_model = new LAppModel(this);
		
		QStringList stringList = _resourcePath.split(QStringLiteral("/"));
		int listLength = stringList.length();
		QString dir = _resourcePath.section(QStringLiteral("/"), 0, listLength - 2) + QStringLiteral("/");
		QString fileName = stringList[listLength - 1];
		if (!_model->LoadAssets(dir.toStdString().c_str(), fileName.toStdString().c_str()))
		{
			stringList = _oldPath.split(QStringLiteral("/"));
			listLength = stringList.length();
			dir = _oldPath.section(QStringLiteral("/"), 0, listLength - 2) + QStringLiteral("/");
			fileName = stringList[listLength - 1];
			_model->LoadAssets(dir.toStdString().c_str(), fileName.toStdString().c_str());
		}

		_model->CreateRenderer(_window->openglContext());

		if (_textureManager == nullptr)
			_textureManager = new LAppTextureManager;
		_textureManager->SetWindow(_window);
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

		_modelUpdated = false;
	}
}

void LAppRenderer::handleVoiceStateChanged(QAudio::State newState)
{
	switch (newState)
	{
	case QAudio::IdleState:
		_audioOutput->stop();
		_audioFile.close();
		break;
	case QAudio::StoppedState:
		if (_audioOutput)
		{
			if (_audioOutput->error() != QAudio::NoError)
			{
				switch (_audioOutput->error())
				{
				case QAudio::OpenError:
					LAppPal::PrintLog("An error occurred opening the audio device.");
					break;
				case QAudio::IOError:
					LAppPal::PrintLog("An error occurred during read/write of audio device.");
					break;
				case QAudio::UnderrunError:
					LAppPal::PrintLog("Audio data is not being fed to the audio device at a fast enough rate.");
					break;
				case QAudio::FatalError:
					LAppPal::PrintLog("A non-recoverable error has occerred, the audio device is not usable at this time.");
					break;
				}
			}
		}
		break;
	}
}
