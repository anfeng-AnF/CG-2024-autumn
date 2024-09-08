#pragma once
#include "Graphics.h"
#include "CollisionGeometry.h"
#include "Camera.h"
#include "map"
#include "DebugLine.h"
#include "DebugSphere.h"
#include "TransformCtrlComponent.h"
#include "Window.h"
#include <thread>

class TransformComponentBase
{
public:
	struct screenPos {
		UINT x;
		UINT y;
	};
	enum TransformAxis {
		NONE_AXIS,
		X,
		Y,
		Z,
		XY,
		XZ,
		YZ,
		XYZ
	};
public:
	TransformComponentBase();
	//Called externally and passed in mouse displacement to get component-controlled transformations
	virtual XMMATRIX GetDeltaTransform(screenPos from,screenPos to,Window&wnd) = 0;
	void draw(Graphics& gfx)const noexcept;
	void SetTransform(FTransform transform);

	//Detects if a line passes through the component and returns whether it hit or not.
	bool TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);
protected:
	FTransform transform;
	std::unique_ptr<TransformCtrlComponent> pTransformCtrlComponent;
};

class TranslateComponent :public TransformComponentBase
{

};

class RotationComponent :public TransformComponentBase
{

};

class ScaleComponent :public TransformComponentBase
{

};