#pragma once

#include <QObject>
#include "LAppAllocator.h"
#include "CubismSDK/Framework/Math/CubismViewMatrix.hpp"

class QQuickWindow;
class LAppModel;
class LAppTextureManager;

/**
 * @brief ʵ���Զ�����Ⱦ����
*/
class LAppRenderer : public QObject
{
	Q_OBJECT

public:
	LAppRenderer();
	~LAppRenderer();

	/**
	 * @brief ��GUI�̴߳���������Ⱦ�����QQuickWindow����
	 * @param window 
	*/
	void setWindow(QQuickWindow* window);

	/**
	 * @brief ������Ⱦ������ʹ�õ���ViewMatrix
	 * @param viewMat 
	*/
	void setViewMatrix(Csm::CubismViewMatrix* viewMat);

	/**
	 * @brief ����Ҫ��Ⱦ��ģ��
	 * @param model 
	*/
	void setModel(LAppModel* model);

	void setTextureManager(LAppTextureManager* mgr);

	void setModelUpdated(bool updated);

	void setResourcePath(QString path);

public slots:
	/**
	 * @brief ��ʼ����Ⱦ������ز���
	*/
	void init();

	/**
	 * @brief ��Ҫ��Ⱦ����
	*/
	void paint();

private:
	Csm::CubismViewMatrix* _viewMatrix;
	LAppModel* _model;
	QQuickWindow* _window;
	bool _modelUpdated;
	LAppTextureManager* _textureManager;
	QString _resourcePath;
};