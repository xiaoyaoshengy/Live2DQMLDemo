#pragma once

#include <QObject>
#include "LAppAllocator.h"
#include "CubismSDK/Framework/Math/CubismViewMatrix.hpp"

class QQuickWindow;
class LAppModel;

/**
 * @brief 实现自定义渲染的类
*/
class LAppRenderer : public QObject
{
	Q_OBJECT

public:
	LAppRenderer();
	~LAppRenderer();

	/**
	 * @brief 从GUI线程传递来的渲染输出的QQuickWindow窗体
	 * @param window 
	*/
	void setWindow(QQuickWindow* window);

	/**
	 * @brief 设置渲染过程中使用到的ViewMatrix
	 * @param viewMat 
	*/
	void setViewMatrix(Csm::CubismViewMatrix* viewMat);

	/**
	 * @brief 设置要渲染的模型
	 * @param model 
	*/
	void setModel(LAppModel* model);

public slots:
	/**
	 * @brief 初始化渲染器的相关操作
	*/
	void init();

	/**
	 * @brief 主要渲染过程
	*/
	void paint();

private:
	Csm::CubismViewMatrix* _viewMatrix;
	LAppModel* _model;
	QQuickWindow* _window;
};