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
#include <thread>
struct screenPos;
DirectX::XMFLOAT3 ConvertScreenToView(screenPos scPos, Window& wnd,float deepthOffset=0.0f);
DirectX::XMFLOAT3 ConvertViewToWorld(XMFLOAT3 viewPos, Window& wnd);
struct screenPos {
		int x;
		int y;
		screenPos& operator+=(const screenPos& other);
		screenPos(std::pair<int, int> pos);
		screenPos(int a, int b) :x(a), y(b) {};
		screenPos() = default;
		bool operator!=(const screenPos& other) const;
		screenPos operator+(const screenPos& other) const;
	};
struct LineRay {
	DirectX::XMFLOAT3 rayOrigin;
	DirectX::XMFLOAT3 rayDirection;
	LineRay() = default;
	LineRay(screenPos sp, Window& wnd,Camera&cam);
	LineRay(DirectX::XMVECTOR origin, DirectX::XMVECTOR direction);
};

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
	TransformComponentBase(Graphics& gfx,Camera&cam , std::string filePath);
	//Called externally and passed in mouse displacement to get component-controlled transformations
	virtual XMMATRIX GetDeltaTransform(screenPos from,screenPos to,Window&wnd) = 0;
	void draw(Graphics& gfx)const noexcept;
	void SetTransform(FTransform transform);

	//Detects if a line passes through the component and returns whether it hit or not.
	bool TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);
	void EndTransform();
protected:
	//Utility function
	LineRay GetPlaneFlatPointNormal(LineRay line,XMVECTOR point);
	XMFLOAT3 GetIntersectionPlaneLine(LineRay plane, LineRay line);//Plane point formula and linear pointwise formula
	float GetProjectionLength(const XMFLOAT3& aDirection, const XMFLOAT3& bDirection);
	float GetProjectionLength(const XMVECTOR& aDirection, const XMVECTOR& bDirection);
	XMMATRIX CreateTranslationMatrix(const XMFLOAT3& translation);
	LineRay GetPlane();
	XMVECTOR XM3F2XMVEC(XMFLOAT3 f3);

	FTransform transform;
	std::unique_ptr<TransformCtrlComponent> pTransformCtrlComponent;

	TransformAxis tAxis=NONE_AXIS;
	cMesh* ctrlingMesh=nullptr;
	
	DebugGraphsMannger& DGM;
	Graphics& gfx;
	Camera& cam;

	XMFLOAT3 fromWorldPos;
	bool onTransform = false;
};

class TranslateComponent :public TransformComponentBase
{
public:
	TranslateComponent(Graphics& gfx, Camera& cam, std::string filePath);
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;
private:
};

class RotationComponent :public TransformComponentBase
{
public:
	RotationComponent(Graphics& gfx, Camera& cam, std::string filePath);
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;

};

class ScaleComponent :public TransformComponentBase
{
public:
	ScaleComponent(Graphics& gfx, Camera& cam, std::string filePath);
	XMMATRIX GetDeltaTransform(screenPos from, screenPos to, Window& wnd) override;
private:
	float GetScaleLength(LineRay Plane, LineRay ray);
private:
	float BeginScaleLength=0.0f;
	XMFLOAT3 scaleDirection;
};

class CollisionGeoManager
{
	struct TransformState;
public:
	enum TransformationMethod
	{
		NONE = 0,
		TRANSLATE = 1,
		SCALE = 2,
		ROTATION = 3
	};
	friend class TranslationState;
	//Interface for Input Control
	class TranslationState : public InputState
	{
	public:
		TranslationState(Window& window, CollisionGeoManager& manager);
		TranslationState(const TranslationState& other) = default;
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;
	private:
		CollisionGeoManager& collisionManager;
		std::unordered_map<char, TransformationMethod>shortcutKey = {
				{'q', TransformationMethod::NONE},
				{'w', TransformationMethod::TRANSLATE},
				{'e', TransformationMethod::SCALE},
				{'r', TransformationMethod::ROTATION},
				{'Q', TransformationMethod::NONE},
				{'W', TransformationMethod::TRANSLATE},
				{'E', TransformationMethod::SCALE},
				{'R', TransformationMethod::ROTATION},
		};
		bool selectedComponent = false;
	}inputState;

public:
	CollisionGeoManager(Window& wnd, Camera& cam);

	void AddGeometry(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	void AddGeometry(std::shared_ptr<CollisionGeometry> Geo, bool isSelected = false);

	//caculate deltaTransform from imgui/component
	void TransformGeometryByImGui(Window& wnd);
	void TransformGeometryByComponent(Window& wnd, screenPos Delta);
	/// <summary>
	/// Selects a geometry based on the provided screen position and window context.
	/// </summary>
	/// <param name="pos">The screen position to check for geometry selection.</param>
	/// <param name="wnd">The window context to use for input handling.</param>
	/// <returns>
	/// <c>0</c> if no geometry is selected.<br/>
	/// <c>1</c> if a collision geometry is selected.<br/>
	/// <c>2</c> if a transform component is selected.
	/// </returns>
	int SelectGeometry(screenPos pos, Window& wnd);

	void ChangeTransformationMethod(TransformationMethod method);
	void Draw(Graphics& gfx);

	int GetSelectedGeoNum();
	//get transform data from ImGui
	void DrawImGui(Graphics& gfx);

	//component transform end
	void EndComponentTransform();
private:
	//apply deltaTransform to Geos
	void Transform();

	//Update control point location
	void RenewOriginPointPos();

	//Renew Data
	void RenewTransformDelta();

	
	void ApplyTransform();
private:
	std::unique_ptr<TranslateComponent> pTranslateComponent;
	std::unique_ptr<RotationComponent> pRotationComponent;
	std::unique_ptr<ScaleComponent> pScaleComponent;

	std::unordered_map<std::shared_ptr<CollisionGeometry>, bool>Geomertys;
	std::list<std::pair<std::shared_ptr<CollisionGeometry>, FTransform>> SelectedGeomertys; //selected geo and pervious transform

	//change by DrawImgui,TransformComponent,reset when SelectedGeometrys size change 
	struct TransformState
	{
		FTransform deltaTransform;		//delta transform
		XMFLOAT3 DeltaRotationEuler;	//delta rotation euler -> quaternion rotation 
		FTransform originTransform;		//origin point transform
		XMFLOAT3 color;					//change color is use
		screenPos beginScreenPos;		//Start controlling the click position when transforming components
		screenPos deltaScreenPos;		//Delta using rawdata overlay
		TransformationMethod transformationMethod;		//0-none  1-translate  2-scale 3-rotation
		bool affectOriginOnly;
		bool autoApplyTransform=true;
	}TransformData;

	DebugGraphsMannger& DGM;
	Graphics* gfx;
	Camera* cam;
};