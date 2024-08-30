#include "CollisionGeometry.h"

CollisionGeomerty::CollisionGeomerty(Graphics& gfx, Dvtx::VertexBuffer &_vertexBuffer, std::vector<uint16_t> _indices):
    vertexBuffer(_vertexBuffer),
    indices(_indices)
{
    using namespace Bind;
    AddBind(VertexBuffer::Resolve(gfx, " ", vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, " ", indices));
    AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(Bind::PixelShader::Resolve(gfx, "CollisionGeomertyPS.cso"));
    
    auto pvs = Bind::VertexShader::Resolve(gfx, "CollisionGeomertyVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), pvsb));

    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX CollisionGeomerty::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

std::string getSubStr(const char* begin, size_t length) {
    std::string ret;
    for (int i = 0; i < length; i++) {
        ret += *begin;
        begin++;
    }
    return ret;
}

std::vector<CollisionGeomerty::CollisionRes> CollisionGeomerty::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
    std::vector<CollisionRes> hitRes;
    namespace dx=DirectX;
    UINT32 indicesSize = indices.size();
    DirectX::XMFLOAT3 p0, p1, p2;
    auto data = vertexBuffer.GetData();
    //cacularte data size
    const auto layout = vertexBuffer.GetLayout().GetD3DLayout();
    const auto layoutSize = vertexBuffer.GetLayout().Size();
    UINT32 offset = 0;
    for (auto type : layout) {
        if (type.SemanticName == Dvtx::VertexLayout::Map<Dvtx::VertexLayout::Position3D>::semantic) {
            offset = type.AlignedByteOffset;
        }
    }
    dx::XMVECTOR v0, v1, v2;
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = { lineVector.x,lineVector.y,lineVector.z,0.0f };

    for (UINT32 i = 0; i < indicesSize; i+=3) {
        //get a triangle 3 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p2 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+2] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x,p0.y,p0.z,0.0f };
        v1 = { p1.x,p1.y,p1.z,0.0f };
        v2 = { p2.x,p2.y,p2.z,0.0f };
        
        auto normal = dx::XMVector3Cross(dx::XMVectorSubtract(v1, v0), dx::XMVectorSubtract(v2, v1));
        auto t = dx::XMVectorGetX(dx::XMVector3Dot(normal, dx::XMVectorSubtract(v0, lineBeginV)))
            /
            dx::XMVectorGetX(dx::XMVector3Dot(normal, lineDirection));
        if (t < 0)continue;
        //get line hit position
        auto HitPosition = dx::XMVectorAdd(lineBeginV, dx::XMVectorScale(lineDirection, t));
        auto C0 = dx::XMVectorSubtract(HitPosition, v0);
        auto C2 = dx::XMVectorSubtract(HitPosition, v2);

        auto N0 = dx::XMVector3Cross(dx::XMVectorSubtract(v1, v0), C0);
        auto N2 = dx::XMVector3Cross(dx::XMVectorSubtract(v2, v1), C2);

        if (dx::XMVectorGetX(dx::XMVector3Dot(N0, normal)) == dx::XMVectorGetX(dx::XMVector3Dot(N2, normal))) {
            hitRes.push_back({
                {
                    dx::XMVectorGetX(HitPosition),
                    dx::XMVectorGetY(HitPosition),
                    dx::XMVectorGetZ(HitPosition),
                },
                {
                    dx::XMVectorGetX(normal),
                    dx::XMVectorGetY(normal),
                    dx::XMVectorGetZ(normal),
                }
                });
        }
    }


    return hitRes;
}

void CollisionGeomerty::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    this->pos = pos;
}
