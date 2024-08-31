#pragma once
#pragma once
#include "Graphics.h"
#include "CollisionGeometry.h"
#include "Camera.h"
#include "map"
#include "Line.h"
#include "DebugSphere.h"
#include "ArrowComponent.h"
#include "Window.h"
#include <thread>
class CtrlGeomerty
{
    using pGeoPair = std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>*;
public:
	CtrlGeomerty(Camera* cam,Graphics&gfx);
	void ChangeCamera(Camera* newCam);
    void Draw();
    void AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
    bool SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight);
    void TransformGeomerty(Window&wnd);

private:
    pGeoPair TraceByLineNearestGeo();
	void TraceByLine(int click_x, int click_y,const int windowWidth, const int windowHeight);

	const Camera* CurrentCamera;
	std::vector<std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>> hitRes;
    std::vector<std::unique_ptr<Drawable>> DebugGraphs;
    Graphics& gfx;

    std::shared_ptr<ArrowComponent> pArrowComponent;
    std::unordered_map<std::shared_ptr<CollisionGeomerty>,bool>Geomertys;
    UINT16 selectedGeo = 0;

private:
    //transform
    void move(Window& wnd);


};


inline CtrlGeomerty::CtrlGeomerty(Camera* cam, Graphics& gfx):CurrentCamera(cam),gfx(gfx)
{

    //pArrowComponent = ArrowComponent::ConstructHelper(gfx, { 1.0f,0.0f,0.0f });
}

inline void CtrlGeomerty::ChangeCamera(Camera* newCam)
{
	CurrentCamera = newCam;
}

inline CtrlGeomerty::pGeoPair CtrlGeomerty::TraceByLineNearestGeo()
{
    std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>* NearestGeo=nullptr;
    float min = D3D11_FLOAT32_MAX;
    for (auto& hit: hitRes) {
        for (auto& hitVal : hit.second) {
            if (min > hitVal.hitDistance) {
                min = hitVal.hitDistance;
                NearestGeo = &hit;
            }
        }
    }
    return NearestGeo;
}

inline void CtrlGeomerty::Draw()
{
    for (const auto& obj : Geomertys) {
        if (auto a = static_cast<CollisionGeomerty*>(obj.first.get())) {
            a->Bind(gfx);
        }
        obj.first->Draw(gfx);
    }
    for (const auto& obj : DebugGraphs) {
        obj->Draw(gfx);
    }
    if (pArrowComponent) {
        pArrowComponent->Draw(gfx);
    }
}

inline void CtrlGeomerty::TraceByLine(int click_x, int click_y,const int windowWidth,const int windowHeight)
{
    // 获取相机的视图矩阵和投影矩阵
    XMMATRIX viewMatrix = CurrentCamera->GetMatrix(); // 视图矩阵
    XMMATRIX projectionMatrix = gfx.GetProjection(); // 投影矩阵

    // 计算标准化设备坐标 (NDC)
    float ndcX = (2.0f * click_x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * click_y) / windowHeight;
    float ndcZ = 1.0f; // 近裁剪面

    // Step 2: 将 NDC 转换为视图空间坐标
    XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);
    XMVECTOR rayNDC = XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);
    XMVECTOR rayViewSpace = XMVector3TransformCoord(rayNDC, invProjMatrix);

    // Step 3: 将视图空间坐标转换为世界空间射线
    XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
    XMVECTOR rayWorldSpace = XMVector3TransformNormal(rayViewSpace, invViewMatrix);

    // 获取相机位置作为射线的起点
    XMVECTOR cameraPosition = invViewMatrix.r[3];

    // 提取射线的起点和方向
    XMFLOAT3 rayOrigin, rayDirection;
    XMStoreFloat3(&rayOrigin, cameraPosition);
    XMStoreFloat3(&rayDirection, XMVector3Normalize(rayWorldSpace));
    auto farpos = DirectX::XMVectorAdd(cameraPosition, DirectX::XMVectorScale(rayWorldSpace, 1e5));
    XMFLOAT3 rayFarPoint;
    XMStoreFloat3(&rayFarPoint, farpos);
    DebugGraphs.push_back(std::make_unique<Line>(gfx, rayOrigin, rayFarPoint, XMFLOAT3{ 1.0f,0.0f,0.0f }));
    this->hitRes.clear();
    for (auto& obj : Geomertys) {
        auto hitResult = obj.first->TraceByLine(rayOrigin, rayDirection);
        if (hitResult.size() == 0)continue;
        this->hitRes.push_back({ obj.first,hitResult });
        for (auto& val : hitResult) {
            this->DebugGraphs.push_back(std::make_unique<DebugSphere>(gfx, XMFLOAT3{ 0.0f,0.0f,1.0f }, val.pos));
        }
    }
}

inline void CtrlGeomerty::move(Window& wnd)
{
    enum moveDir
    {
        x,y,z
    }moveStatue;
    moveStatue = x;
    bool onMove = true;
    while (onMove)
    {
        if (wnd.mouse.LeftIsPressed()) {
            bool selected = this->SelectGeomerty(wnd.mouse.GetPosX(), wnd.mouse.GetPosY(), wnd.GetWndSize().first, wnd.GetWndSize().second);
            if (!selected) {
                onMove = false;
                continue;
            }
        }
        if (wnd.Kbd.KeyIsPressed('x'))moveStatue = x;
        if (wnd.Kbd.KeyIsPressed('y'))moveStatue = y;
        if (wnd.Kbd.KeyIsPressed('z'))moveStatue = z;
        auto rawDelta = wnd.mouse.ReadRawDelta();
        if (!rawDelta)continue;
        for (auto& Geo : Geomertys) {
            if (Geo.second) {
                switch (moveStatue)
                {
                case x:
                    Geo.first->SetPos({ Geo.first->GetPos().x * rawDelta->x,Geo.first->GetPos().y,Geo.first->GetPos().z });
                    break;
                case y:
                    break;
                case z:
                    break;
                }
            }
        }
    }
}

inline void CtrlGeomerty::AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
    Geomertys[std::make_shared<CollisionGeomerty>(gfx, _vertexBuffer, _indices, _pos)] = false;
}

inline bool CtrlGeomerty::SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight)
{
    TraceByLine(click_x, click_y, windowWidth, windowHeight);
    pGeoPair pNearestGeo = TraceByLineNearestGeo();

    if (!pNearestGeo) {
        for (auto& geo : Geomertys) {
            if (geo.second) {
                geo.first->SetSelect(false);
                geo.second = false;
            }
        }
        return false;
    }
    if (Geomertys[pNearestGeo->first]) {
        Geomertys[pNearestGeo->first] = false;
        pNearestGeo->first->SetSelect(false);
        selectedGeo--;
    }
    else
    {
        Geomertys[pNearestGeo->first] = true;
        pNearestGeo->first->SetSelect(true);
        selectedGeo++;
    }
    return true;
}

inline void CtrlGeomerty::TransformGeomerty(Window& wnd)
{

    enum transformStatue
    {
        Selecting,
        Move,
        Scale,
        Rotate
    }currentStatue;
    currentStatue = Selecting;
    while (selectedGeo)
    {
        if (wnd.CursorEnabled()) {
            if (wnd.Kbd.KeyIsPressed('W')) {
                currentStatue = Move;
            }
            if (wnd.Kbd.KeyIsPressed('G')) {
                currentStatue = Move;
            }
            if (wnd.Kbd.KeyIsPressed('S')) {
                currentStatue = Scale;
            }
            if (wnd.Kbd.KeyIsPressed('R')) {
                currentStatue = Rotate;
            }

            switch (currentStatue)
            {
            case Selecting:
                break;
            case Move:
                move(wnd);
                break;
            case Scale:
                break;
            case Rotate:
                break;
            }
        }
    }
}

