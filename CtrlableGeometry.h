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
#include "list"
#include "InputState.h"
#include <thread>

struct screenPos {
		UINT x;
		UINT y;
	};
struct LineRay {
	XMFLOAT3 rayOrigin;
	XMFLOAT3 rayDirection;
	LineRay() = default;
	LineRay(screenPos sp, Window& wnd) {

	};
};
XMFLOAT3 ConvertScreenToView(screenPos scPos, Window& wnd,float deepthOffset=0.0f);
XMFLOAT3 ConvertViewToWorld(XMFLOAT3 viewPos, Window& wnd);


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
	void EndTransform();
protected:
	FTransform transform;
	std::unique_ptr<TransformCtrlComponent> pTransformCtrlComponent;

	TransformAxis tAxis;
	cMesh* ctrlingMesh=nullptr;
	
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

class CollisionGeoManager
{
public:
	void AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	void AddGeomerty(std::shared_ptr<CollisionGeometry> Geo,bool isSelected=false);

	void TransformGeometryByImGui(Window& wnd);
	void TransformGeometryByComponent(screenPos pos);

	auto SelectGeometry(screenPos pos, Window& wnd);

	void Draw(Graphics& gfx);
private:
	std::unique_ptr<TranslateComponent> translateComponent;
	std::unique_ptr<RotationComponent> rotationComponent;
	std::unique_ptr<ScaleComponent> scaleComponent;

	std::unordered_map<std::shared_ptr<CollisionGeometry>, bool>Geomertys;
	std::list<std::shared_ptr<CollisionGeometry>, FTransform>mapSelectedGeomertys; //selected geo and pervious transform

	struct 
	{
		FTransform deltaTransform;
		FTransform originTransform;
		XMFLOAT3 color;
	}ImGuiTransformData;

	DebugGraphsMannger& DGM = DebugGraphsMannger::GetDGMRefference();
};


namespace InputStateClass {
	class UsingTransformComponent :public InputState
	{
		// Í¨¹ý InputState ¼Ì³Ð
		void Enter() override;
		void HandleKeyboardInput() override;
		void HandleMouseInput() override;
		void Update(float deltaTime) override;
		void Exit() override;
	};
}