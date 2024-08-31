#pragma once
#pragma once
#include "Graphics.h"
#include "CollisionGeometry.h"
#include "Camera.h"
#include "map"
#include "Line.h"
#include "DebugSphere.h"

class CtrlCeomerty
{
public:
	CtrlCeomerty(Camera* cam,Graphics&gfx);
	void ChangeCamera(Camera* newCam);
	//CollisionGeomerty* TraceByLineRecentGeo(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);
    void Draw();
	void TraceByLine(int click_x, int click_y,const int windowWidth, const int windowHeight);
    void AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
private:
	std::vector<std::shared_ptr<CollisionGeomerty>> Geomertys;
	const Camera* CurrentCamera;
	std::vector<std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>> hitRes;
    std::vector<std::unique_ptr<Drawable>> DebugGraphs;
    Graphics& gfx;
};


inline CtrlCeomerty::CtrlCeomerty(Camera* cam, Graphics& gfx):CurrentCamera(cam),gfx(gfx)
{

}

inline void CtrlCeomerty::ChangeCamera(Camera* newCam)
{
	CurrentCamera = newCam;
}

inline void CtrlCeomerty::Draw()
{
    for (const auto& obj : Geomertys) {
        obj->Draw(gfx);
    }
    for (const auto& obj : DebugGraphs) {
        obj->Draw(gfx);
    }
}

inline void CtrlCeomerty::TraceByLine(int click_x, int click_y,const int windowWidth,const int windowHeight)
{
    // ��ȡ�������ͼ�����ͶӰ����
    XMMATRIX viewMatrix = CurrentCamera->GetMatrix(); // ��ͼ����
    XMMATRIX projectionMatrix = gfx.GetProjection(); // ͶӰ����

    // �����׼���豸���� (NDC)
    float ndcX = (2.0f * click_x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * click_y) / windowHeight;
    float ndcZ = 1.0f; // ���ü���

    // Step 2: �� NDC ת��Ϊ��ͼ�ռ�����
    XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);
    XMVECTOR rayNDC = XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);
    XMVECTOR rayViewSpace = XMVector3TransformCoord(rayNDC, invProjMatrix);

    // Step 3: ����ͼ�ռ�����ת��Ϊ����ռ�����
    XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
    XMVECTOR rayWorldSpace = XMVector3TransformNormal(rayViewSpace, invViewMatrix);

    // ��ȡ���λ����Ϊ���ߵ����
    XMVECTOR cameraPosition = invViewMatrix.r[3];

    // ��ȡ���ߵ����ͷ���
    XMFLOAT3 rayOrigin, rayDirection;
    XMStoreFloat3(&rayOrigin, cameraPosition);
    XMStoreFloat3(&rayDirection, XMVector3Normalize(rayWorldSpace));
    auto farpos = DirectX::XMVectorAdd(cameraPosition, DirectX::XMVectorScale(rayWorldSpace, 1e5));
    XMFLOAT3 rayFarPoint;
    XMStoreFloat3(&rayFarPoint, farpos);
    DebugGraphs.push_back(std::make_unique<Line>(gfx, rayOrigin, rayFarPoint, XMFLOAT3{ 1.0f,0.0f,0.0f }));

    for (auto& obj : Geomertys) {
        auto hitResult = obj->TraceByLine(rayOrigin, rayDirection);
        this->hitRes.push_back({ obj,hitResult });
        for (auto val : hitResult) {
            this->DebugGraphs.push_back(std::make_unique<DebugSphere>(gfx, XMFLOAT3{ 0.0f,0.0f,1.0f }, val.pos));
        }
    }
}

inline void CtrlCeomerty::AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
    Geomertys.push_back(std::make_shared<CollisionGeomerty>(gfx,_vertexBuffer,_indices,_pos));
}
