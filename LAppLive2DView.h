#pragma once

#include <QQuickItem>
#include "Framework/Math/CubismMatrix44.hpp"
#include "Framework/Math/CubismViewMatrix.hpp"
#include "Framework/CubismFramework.hpp"
#include "LAppAllocator.h"

class LAppRenderer;
class TouchManager;

class LAppLive2DView : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
	Q_PROPERTY(QString resourcePath READ resourcePath WRITE setResourcePath NOTIFY resourcePathChanged)

public:
	LAppLive2DView(QQuickItem* parent = nullptr);

	~LAppLive2DView();

	void setT(qreal t);

	qreal t() const { return _t; }

	void setResourcePath(QString path);

	QString resourcePath() const { return _resourcePath; }

	Q_INVOKABLE void handleMousePressEvent(int button, double x, double y);

	Q_INVOKABLE void handleMouseMoveEvent(int button, double x, double y);

	Q_INVOKABLE void handleMouseReleaseEvent(int button, double x, double y);

signals:
	void tChanged();
	void resourcePathChanged();

public slots:
	void sync();

	void cleanup();

protected:
	void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
	void handleWindowChanged(QQuickWindow* win);

private:
	void releaseResources() override;

	void CalculateMatrix();

	void InitializeCubism();

    qreal _t;
    bool _capture;
	bool _modelUpdated;
    QString _resourcePath;
    LAppRenderer* _renderer;
    TouchManager* _touchManager;
    Csm::CubismMatrix44* _deviceToScreen;
    Csm::CubismViewMatrix* _viewMatrix;
    Csm::CubismFramework::Option _cubismOption;
    LAppAllocator _cubismAllocator;
};
