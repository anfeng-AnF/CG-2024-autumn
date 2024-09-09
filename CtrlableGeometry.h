#pragma once
#include "Graphics.h"
#include "CollisionGeometry.h"
#include "Camera.h"
#include "map"
#include "DebugLine.h"
#include "DebugSphere.h"
#include "TransformCtrlComponent.h"
#include "Window.h"
#include "DebugGraphsMannger.h"
#include <thread>

struct screenPos {
		UINT x;
		UINT y;
	};
void ConvertScreenToWorld(screenPos scPos, Window& wnd);

class TransformComponentBase
{
public:
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
	TransformAxis tAxis;
	DebugGraphsMannger& DGM;
	mutable Graphics* pGfx = nullptr;
};

class TranslateComponent :public TransformComponentBase
{
public:
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;
};

class RotationComponent :public TransformComponentBase
{
public:
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;
};

class ScaleComponent :public TransformComponentBase
{
public:
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;
};