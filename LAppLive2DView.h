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
 * @brief �̳�QQuickItem��ʵ���Զ���QML���
*/
class LAppLive2DView : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
	Q_PROPERTY(QString resourcePath READ resourcePath WRITE setResourcePath NOTIFY resourcePathChanged)

public:
	/**
	 * @brief ���캯��
	*/
	LAppLive2DView(QQuickItem* parent = nullptr);

	/**
	 * @brief ��������
	 */
	~LAppLive2DView();

	/**
	 * @brief ��������t��ֵ
	 * @param t Ҫ���õ�ֵ
	*/
	void setT(qreal t);

	/**
	 * @brief ��ȡ����t��ֵ
	 * @return qreal����
	*/
	qreal t() const { return _t; }

	/**
	 * @brief ��������resourcePath
	 * ������ΪLive2Dģ�������ļ�(.model3.json�ļ�)��·��
	 * ע����ʹ�þ���·��
	 * @param path �ļ�·��
	*/
	void setResourcePath(QString path);

	/**
	 * @brief ��ȡ����resourcePath
	 * @return QString����
	*/
	QString resourcePath() const { return _resourcePath; }

signals:
	void tChanged();
	void resourcePathChanged();

public slots:
	/**
	 * @brief ֱ����QQuickWindow::beforeSynchronization�ź�
	 * ��������Ⱦ�̴߳��ݲ���
	*/
	void sync();

	/**
	 * @brief ֱ����QQuickWindow::sceneGraphInvalidated�ź�
	 * �����ڳ���ͼʧЧ���ͷ���Ⱦ�߳��ϵ���Դ
	*/
	void cleanup();

protected:
	/**
	 * @brief ��д��갴���¼�
	 * @param ev 
	*/
	void mousePressEvent(QMouseEvent* ev) override;

	/**
	 * @brief ��д����ƶ��¼�
	 * @param ev 
	*/
	void mouseMoveEvent(QMouseEvent* ev) override;

	/**
	 * @brief ��д����ͷ��¼�
	 * @param ev 
	*/
	void mouseReleaseEvent(QMouseEvent* ev) override;

	/**
	 * @brief ��д������Ϣ�����¼�
	 * @param newGeometry 
	 * @param oldGeometry 
	*/
	void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
	/**
	 * @brief ����QQuickItem::windowChanged�ź�
	*/
	void handleWindowChanged(QQuickWindow* win);

	/**
	 * @brief ֱ��QQuickWindow::sceneGraphInitialied�ź�
	 * ����������ͼ��ز���
	*/
	void setupTextures();

private:
	/**
	 * @brief ��д��Դ�ͷŷ���
	*/
	void releaseResources() override;

	/**
	 * @brief ��ʼ������ת��������صľ���
	*/
	void CalculateMatrix();

	/**
	 * @brief ��ʼ��CubismSDK
	*/
	void InitializeCubism();

	qreal _t;										///< ���ڴ洢����t��ֵ
	bool _capture;									///< ��ʾ�����Ƿ񱻲����״̬
	QString _resourcePath;							///< ���ڴ洢����resourcePath��ֵ
	LAppRenderer* _renderer;						///< ��Ⱦ��
	LAppModel* _model;								///< ģ�������Ϣ�Ͳ����Ķ���
	TouchManager* _touchManager;					///< ���ڹ������¼�
	Csm::CubismMatrix44* _deviceToScreen;			///< ����ת������ľ���
	Csm::CubismViewMatrix* _viewMatrix;				///< ����ת������ľ���
	Csm::CubismFramework::Option _cubismOption;		///< Cubism Option
	LAppAllocator _cubismAllocator;					///< Cubism Allocator
	LAppTextureManager* _textureManager;			///< ���ڹ���������ͼ
};