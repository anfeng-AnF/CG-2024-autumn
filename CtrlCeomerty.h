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
    bool SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight, bool isPerspective=true);
    void TransformGeomerty(Window&wnd);

private:
    pGeoPair TraceByLineNearestGeo();
	void TraceByLine(int click_x, int click_y,const int windowWidth, const int windowHeight, bool isPerspective=true);

	const Camera* CurrentCamera;
	std::vector<std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>> hitRes;
    std::vector<std::unique_ptr<Drawable>> DebugGraphs;
    Graphics& gfx;

    std::shared_ptr<ArrowComponent> pArrowComponent;
    std::unordered_map<std::shared_ptr<CollisionGeomerty>, bool>Geomertys;
    std::unordered_map<std::shared_ptr<CollisionGeomerty>,FTransform>vSelectedGeomertys; //selected geo and pervious transform
    UINT16 selectedGeoNum = 0;
    FTransform deltaTransform;
    XMFLOAT3 DeltaRotationEuler;
private:
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

inline void CtrlGeomerty::TraceByLine(int click_x, int click_y,const int windowWidth,const int windowHeight, bool isPerspective)
{
    // 获取相机的视图矩阵和投影矩阵
    XMMATRIX viewMatrix = CurrentCamera->GetMatrix(); // 视图矩阵
    XMMATRIX projectionMatrix = gfx.GetProjection(); // 投影矩阵
    XMFLOAT3 rayOrigin, rayDirection;

    // 计算标准化设备坐标 (NDC)
    float ndcX = (2.0f * click_x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * click_y) / windowHeight;
    float ndcZ = 1.0f; // 对于透视投影

    XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
    XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);
    if (isPerspective) {
        // Step 2: 将 NDC 转换为视图空间坐标
        XMVECTOR rayNDC = XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);
        XMVECTOR rayViewSpace = XMVector3TransformCoord(rayNDC, invProjMatrix);

        // Step 3: 将视图空间坐标转换为世界空间射线
        XMVECTOR rayWorldSpace = XMVector3TransformCoord(rayViewSpace, invViewMatrix);

        // 获取相机位置作为射线的起点
        XMVECTOR cameraPosition = invViewMatrix.r[3];
        // 透视视图
        rayWorldSpace = XMVector3Normalize(rayWorldSpace);
        XMStoreFloat3(&rayDirection, rayWorldSpace);
        XMStoreFloat3(&rayOrigin, cameraPosition);
    }
    else {
        rayDirection = CurrentCamera->GetForwardVector();
        XMStoreFloat3(&rayOrigin, CurrentCamera->GetTransform().position);
    }
#ifdef _DEBUG
    //create debug line
    XMFLOAT3 rayFarPoint = {10000*rayDirection.x,10000 * rayDirection.y ,10000 * rayDirection.z };
    DebugGraphs.push_back(std::make_unique<Line>(gfx, rayOrigin, rayFarPoint, XMFLOAT3{ 1.0f,0.0f,0.0f }));
#endif // _DEBUG
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

inline void CtrlGeomerty::AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
    Geomertys[std::make_shared<CollisionGeomerty>(gfx, _vertexBuffer, _indices, _pos)] = false;
}

inline bool CtrlGeomerty::SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight,bool isPerspective)
{
    //everytime we select,whither selected or not ,we renew old geo transform,and reset deltaTransform
    deltaTransform = FTransform{};
    DeltaRotationEuler = { 0.0f,0.0f,0.0f };
    for (auto& val : vSelectedGeomertys) {
        val.second = val.first->GetTransform();
    }


    TraceByLine(click_x, click_y, windowWidth, windowHeight, isPerspective);
    pGeoPair pNearestGeo = TraceByLineNearestGeo();

    if (!pNearestGeo) {
        vSelectedGeomertys.clear();
        for (auto& geo : Geomertys) {
            if (geo.second) {
                geo.first->SetSelect(false);
                geo.second = false;
                selectedGeoNum--;
            }
        }
        return false;
    }
    if (Geomertys[pNearestGeo->first]) {
        Geomertys[pNearestGeo->first] = false;
        pNearestGeo->first->SetSelect(false);
        selectedGeoNum--;
        vSelectedGeomertys.erase(pNearestGeo->first);
    }
    else
    {
        Geomertys[pNearestGeo->first] = true;
        pNearestGeo->first->SetSelect(true);
        selectedGeoNum++;
        auto a=vSelectedGeomertys.find(pNearestGeo->first);
        vSelectedGeomertys[pNearestGeo->first]=pNearestGeo->first->GetTransform();
    }
    return true;
}

inline void CtrlGeomerty::TransformGeomerty(Window& wnd)
{
    std::ostringstream oss;
    for (auto& val : vSelectedGeomertys) {

    }
    ImGui::BeginChild("transform", { 0,0 });
    if (selectedGeoNum == 0) {
        ImGui::EndChild();
        return;
    }
    ImGui::Text("Position");
    ImGui::SliderFloat("p X", &reinterpret_cast<float*>(&deltaTransform.position)[0], -100.0f, 100.0f);
    ImGui::SliderFloat("p Y", &reinterpret_cast<float*>(&deltaTransform.position)[1], -100.0f, 100.0f);
    ImGui::SliderFloat("p Z", &reinterpret_cast<float*>(&deltaTransform.position)[2], -100.0f, 100.0f);
    ImGui::Text("Scale");
    ImGui::SliderFloat("s X", &reinterpret_cast<float*>(&deltaTransform.scale)[0], -10.0f, 100.0f);
    ImGui::SliderFloat("s Y", &reinterpret_cast<float*>(&deltaTransform.scale)[1], -10.0f, 100.0f);
    ImGui::SliderFloat("s Z", &reinterpret_cast<float*>(&deltaTransform.scale)[2], -10.0f, 100.0f);
    ImGui::Text("Rotation");
    ImGui::SliderFloat("r X", &DeltaRotationEuler.x, -180.0f, 180.0f);
    ImGui::SliderFloat("r Y", &DeltaRotationEuler.y, -180.0f, 180.0f);
    ImGui::SliderFloat("r Z", &DeltaRotationEuler.z, -180.0f, 180.0f);
    ImGui::EndChild();
    deltaTransform.rotation = XMQuaternionRotationRollPitchYaw(DeltaRotationEuler.x, DeltaRotationEuler.y, DeltaRotationEuler.z);
    auto a = vSelectedGeomertys.size();
    for (auto& val : vSelectedGeomertys) {
        FTransform newTransform = {
            XMVectorAdd(val.second.position,deltaTransform.position),
            XMVectorMultiply(val.second.scale,deltaTransform.scale),
            XMQuaternionMultiply(val.second.rotation,deltaTransform.rotation)
            };
        val.first->SetTransform(newTransform);
    }
}

