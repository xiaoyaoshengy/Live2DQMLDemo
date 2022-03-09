#pragma once

#include <QObject>
#include "LAppAllocator.h"
#include "Framework/Math/CubismViewMatrix.hpp"
#include "Framework/Motion/ACubismMotion.hpp"
#include <QAudio>
#include <QFile>

class QQuickWindow;
class LAppModel;
class LAppTextureManager;
class QAudioOutput;

class LAppRenderer : public QObject
{
	Q_OBJECT

public:
	LAppRenderer();
	~LAppRenderer();

	void setWindow(QQuickWindow* window);

	void setViewMatrix(Csm::CubismViewMatrix* viewMat);

	void setModelUpdated(bool updated);

	void setResourcePath(QString path);

	void SetDragging(float px, float py);

	bool HitTest(const char* hitAreaName, float px, float py);

	void SetRandomExpression();

	void StartRandomMotion(const char* group, int priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionCallback);

	void PlayVoice(QString voiceFilePath);

public slots:
	void init();

	void paint();

	void handleVoiceStateChanged(QAudio::State state);

private:
	Csm::CubismViewMatrix* _viewMatrix;
	LAppModel* _model;
	QQuickWindow* _window;
	bool _modelUpdated;
	LAppTextureManager* _textureManager;
	QString _resourcePath;
	QString _oldPath;
	QFile _audioFile;
	QAudioOutput* _audioOutput;
};
