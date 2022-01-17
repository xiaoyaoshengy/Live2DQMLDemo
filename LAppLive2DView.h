#pragma once

#include <QQuickItem>
#include "CubismSDK/Framework/Math/CubismMatrix44.hpp"
#include "CubismSDK/Framework/Math/CubismViewMatrix.hpp"
#include "CubismSDK/Framework/CubismFramework.hpp"
#include "LAppAllocator.h"

class LAppRenderer;
class LAppModel;
class TouchManager;
class LAppTextureManager;

/**
 * @brief 继承QQuickItem以实现自定义QML组件
*/
class LAppLive2DView : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
	Q_PROPERTY(QString resourcePath READ resourcePath WRITE setResourcePath NOTIFY resourcePathChanged)

public:
	/**
	 * @brief 构造函数
	*/
	LAppLive2DView(QQuickItem* parent = nullptr);

	/**
	 * @brief 析构函数
	 */
	~LAppLive2DView();

	/**
	 * @brief 设置属性t的值
	 * @param t 要设置的值
	*/
	void setT(qreal t);

	/**
	 * @brief 获取属性t的值
	 * @return qreal类型
	*/
	qreal t() const { return _t; }

	/**
	 * @brief 设置属性resourcePath
	 * 该属性为Live2D模型配置文件(.model3.json文件)的路径
	 * 注：请使用绝对路径
	 * @param path 文件路径
	*/
	void setResourcePath(QString path);

	/**
	 * @brief 获取属性resourcePath
	 * @return QString类型
	*/
	QString resourcePath() const { return _resourcePath; }

signals:
	void tChanged();
	void resourcePathChanged();

public slots:
	/**
	 * @brief 直连绑定QQuickWindow::beforeSynchronization信号
	 * 用于向渲染线程传递参数
	*/
	void sync();

	/**
	 * @brief 直连绑定QQuickWindow::sceneGraphInvalidated信号
	 * 用于在场景图失效后释放渲染线程上的资源
	*/
	void cleanup();

protected:
	/**
	 * @brief 重写鼠标按下事件
	 * @param ev 
	*/
	void mousePressEvent(QMouseEvent* ev) override;

	/**
	 * @brief 重写鼠标移动事件
	 * @param ev 
	*/
	void mouseMoveEvent(QMouseEvent* ev) override;

	/**
	 * @brief 重写鼠标释放事件
	 * @param ev 
	*/
	void mouseReleaseEvent(QMouseEvent* ev) override;

	/**
	 * @brief 重写几何信息更改事件
	 * @param newGeometry 
	 * @param oldGeometry 
	*/
	void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
	/**
	 * @brief 连接QQuickItem::windowChanged信号
	*/
	void handleWindowChanged(QQuickWindow* win);

	/**
	 * @brief 直连QQuickWindow::sceneGraphInitialied信号
	 * 加载纹理贴图相关操作
	*/
	void setupTextures();

private:
	/**
	 * @brief 重写资源释放方法
	*/
	void releaseResources() override;

	/**
	 * @brief 初始化用于转换计算相关的矩阵
	*/
	void CalculateMatrix();

	/**
	 * @brief 初始化CubismSDK
	*/
	void InitializeCubism();

	qreal _t;										///< 用于存储属性t的值
	bool _capture;									///< 表示窗体是否被捕获的状态
	QString _resourcePath;							///< 用于存储属性resourcePath的值
	LAppRenderer* _renderer;						///< 渲染器
	LAppModel* _model;								///< 模型相关信息和操作的对象
	TouchManager* _touchManager;					///< 用于管理触摸事件
	Csm::CubismMatrix44* _deviceToScreen;			///< 用于转换计算的矩阵
	Csm::CubismViewMatrix* _viewMatrix;				///< 用于转换计算的矩阵
	Csm::CubismFramework::Option _cubismOption;		///< Cubism Option
	LAppAllocator _cubismAllocator;					///< Cubism Allocator
	LAppTextureManager* _textureManager;			///< 用于管理纹理贴图
};