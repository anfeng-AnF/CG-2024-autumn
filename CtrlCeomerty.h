#pragma once
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


class CtrlComponents
{
public:
    enum Statue
    {
        NONE,
        ON_TRANSLATION,
        ON_SCALE,
        ON_ROTATION
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
    CtrlComponents(Graphics& gfx, Camera&cam,
        std::string transFilePath =  "Models\\Component\\Position.fbx",
        std::string scaleFilePath =  "Models\\Component\\Scale.fbx",
        std::string rotateFilePath = "Models\\Component\\Rotation.fbx");
    void Draw(Graphics& gfx)noexcept;
    void SetStatue(CtrlComponents::Statue curStatue);
    XMMATRIX Transform(std::pair<int, int>dTransformPosScreen, int wndWidth, int wndHeight);
    void BeginTransform(std::pair<int, int>bTransformPosScreen, int wndWidth, int wndHeight);
    void EndTransform();

    FTransform GetDeltaTransform();
    void SetTransform(FTransform trans);
    FTransform GetBeginTransform();
    void SetBeginTransform(FTransform trans);

    bool TraceByLineSelectTransformAxis(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);

    std::pair<XMFLOAT3, XMFLOAT3> ScreenToWorldRay(const std::pair<int, int>& screenPos,int wndWidth,int wndHeight,const XMMATRIX& viewMatrix,const XMMATRIX& projectionMatrix,bool isPerspective);
    std::optional<XMFLOAT3> RayIntersectsPlane(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, const XMVECTOR& planePointVec, const XMVECTOR& planeNormalVec);
private:
    XMMATRIX Translation(XMVECTOR delta);
    XMMATRIX Scale(XMVECTOR delta);
    XMMATRIX Rotation(int wndWidth, int wndHeight);
    XMVECTOR ScreenToWorld(std::pair<int, int>deltaPosScreen, int wndWidth, int wndHeight);

private:
    std::unique_ptr<TransformCtrlComponent> pTranslation;
    std::unique_ptr<TransformCtrlComponent> pScale;
    std::unique_ptr<TransformCtrlComponent> pRotation;
    Camera& cam;
    Graphics& gfx;

    Statue currentStatue =Statue::NONE;
    TransformAxis transformAxis=TransformAxis::NONE_AXIS;
    std::pair<int, int>beginTransformPosScreen;
    std::pair<int, int>deltaTransfeomPosScreen;
    XMVECTOR BeginPosWorld;
    FTransform transform;
    FTransform beginTransform;

    bool isInitialized = false;
    //for Rotation
    XMFLOAT3 BeginRotationPosWorld;
};

class CtrlGeomerty
{
    using pGeoPair = std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>*;
public:
	CtrlGeomerty(Camera* cam,Graphics&gfx);
	void ChangeCamera(Camera* newCam);
    void Draw();
    void AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
    void AddGeomerty(std::shared_ptr<CollisionGeomerty> Geo);
    bool SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight, bool isPerspective=true);
    void TransformGeomerty(Window&wnd);

    void ChangeTransformationMethod(const char buffer);

    void EndComponentTransform();
    bool IsUseCtrlComponent();
    void OnTransformComponent(std::pair<int, int> screenPos, int wndWidth, int wndHeight);
private:
    pGeoPair TraceByLineNearestGeo();
	void TraceByLine(int click_x, int click_y,const int windowWidth, const int windowHeight, bool isPerspective=true);

	const Camera* CurrentCamera;
	std::vector<std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>> hitRes;
    std::vector<std::unique_ptr<Drawable>> DebugGraphs;
    Graphics& gfx;

    std::unordered_map<std::shared_ptr<CollisionGeomerty>, bool>Geomertys;
    std::unordered_map<std::shared_ptr<CollisionGeomerty>,FTransform>mapSelectedGeomertys; //selected geo and pervious transform
    UINT16 selectedGeoNum = 0;
    FTransform deltaTransform;
    XMFLOAT3 DeltaRotationEuler;
    XMFLOAT3 color = { -1.0f,-1.0f,-1.0f };

    // References for transformations
    // such as multiple geometries rotating around a point
    // rotating reference systems to translate in different directions
    FTransform currentTransformReference;

    CtrlComponents ctrlComponent;
    bool isUseCtrlComponent;
    int transformationMethod = 0;
private:
};

