#pragma once
class TouchManager
{
public:
	TouchManager();

	float GetCenterX() const { return _lastX; }
	float GetCenterY() const { return _lastY; }
	float GetDeltaX() const { return _deltaX; }
	float GetDeltaY() const { return _deltaY; }
	float GetStartX() const { return _startX; }
	float GetStartY() const { return _startY; }
	float GetScale() const { return _scale; }
	float GetX() const { return _lastX; }
	float GetY() const { return _lastY; }
	float GetX1() const { return _lastX1; }
	float GetY1() const { return _lastY1; }
	float GetX2() const { return _lastX2; }
	float GetY2() const { return _lastY2; }
	bool IsSingleTouch() const { return _touchSingle; }
	bool IsFlickAvailable() const { return _flipAvailable; }
	void DisableFlick() { _flipAvailable = false; }

	void TouchesBegan(float deviceX, float deviceY);
	void TouchesMoved(float deviceX, float deviceY);
	void TouchesMoved(float deviceX1, float deviceY1, float deviceX2, float deviceY2);
	float GetFlickDistance() const;

private:
	float CalculateDistance(float x1, float y1, float x2, float y2) const;
	float CalculateMovingAmount(float v1, float v2);

	float _startY;
	float _startX;
	float _lastX;
	float _lastY;
	float _lastX1;
	float _lastY1;
	float _lastX2;
	float _lastY2;
	float _lastTouchDistance;
	float _deltaX;
	float _deltaY;
	float _scale;
	bool _touchSingle;
	bool _flipAvailable;
};

