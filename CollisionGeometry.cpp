#include "CollisionGeometry.h"

CollisionGeomerty::CollisionGeomerty(Graphics& gfx, Dvtx::VertexBuffer &_vertexBuffer, std::vector<uint16_t> _indices,DirectX::XMFLOAT3 _pos):
    vertexBuffer(_vertexBuffer),
    indices(_indices),
    pos(_pos)
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
    namespace dx=DirectX;
    std::vector<CollisionRes> result;
    UINT32 indicesSize = indices.size();
    DirectX::XMFLOAT3 p0, p1, p2;
    auto data = vertexBuffer.GetData();
    //cacularte data size
    const auto layout = vertexBuffer.GetLayout().GetD3DLayout();
    const auto layoutSize = vertexBuffer.GetLayout().Size();
    UINT32 offset = 0;
    for (const auto& type : layout) {
        if (type.SemanticName == Dvtx::VertexLayout::Map<Dvtx::VertexLayout::Position3D>::semantic) {
            offset = type.AlignedByteOffset;
            break;
        }
    }
    dx::XMVECTOR v0, v1, v2;
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = dx::XMVector3Normalize({ lineVector.x,lineVector.y,lineVector.z,0.0f });

    for (UINT32 i = 0; i < indicesSize; i+=3) {
        //get a triangle 3 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p2 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+2] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x,p0.y,p0.z,0.0f };
        v1 = { p1.x,p1.y,p1.z,0.0f };
        v2 = { p2.x,p2.y,p2.z,0.0f };

        dx::XMVECTOR edge1 = dx::XMVectorSubtract(v1, v0);
        dx::XMVECTOR edge2 = dx::XMVectorSubtract(v2, v0);
        dx::XMVECTOR h = dx::XMVector3Cross(lineDirection, edge2);
        float a = dx::XMVectorGetX(dx::XMVector3Dot(edge1, h));
        if (a > -0.00001f && a < 0.00001f)
            continue;
        float f = 1.0f / a;
        dx::XMVECTOR s = dx::XMVectorSubtract(lineBeginV, v0);
        float u = f * dx::XMVectorGetX(dx::XMVector3Dot(s, h));
        if (u < 0.0f || u > 1.0f)
            continue;
        dx::XMVECTOR q = dx::XMVector3Cross(s, edge1);
        float v = f * dx::XMVectorGetX(dx::XMVector3Dot(lineDirection, q));
        if (v < 0.0f || u + v > 1.0f)
            continue;
        float t = f * dx::XMVectorGetX(dx::XMVector3Dot(edge2, q));

        if (t > 0.00001f)
        {
            dx::XMVECTOR hitPos =dx::XMVectorAdd(lineBeginV ,dx::XMVectorScale(lineDirection,t));
            dx::XMVECTOR normal = dx::XMVector3Normalize(dx::XMVector3Cross(edge1, edge2));
            result.push_back({
                {
                dx::XMVectorGetX(hitPos),
                dx::XMVectorGetY(hitPos),
                dx::XMVectorGetZ(hitPos),
                },
                {
                dx::XMVectorGetX(normal),
                dx::XMVectorGetY(normal),
                dx::XMVectorGetZ(normal),
                },
                {indices[i],indices[i + 1],indices[i + 2]},
                t
                });
        }          

    }
    return result;
}

void CollisionGeomerty::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    this->pos = pos;
}
