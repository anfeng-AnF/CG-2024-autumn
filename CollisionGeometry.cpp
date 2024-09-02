#include "CollisionGeometry.h"


CollisionGeomerty::CollisionGeomerty(Graphics& gfx, Dvtx::VertexBuffer &_vertexBuffer, std::vector<uint16_t> _indices,DirectX::XMFLOAT3 _pos):
    vertexBuffer(_vertexBuffer),
    indices(_indices),
    transform(_pos),
    pCBufColor(gfx,3)
{

}

DirectX::XMMATRIX CollisionGeomerty::GetTransformXM() const noexcept
{
    return transform.GetMatrix();
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
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = dx::XMVector3Normalize({ lineVector.x,lineVector.y,lineVector.z,0.0f });
    XMFLOAT3 pos;
    XMStoreFloat3(&pos, transform.position);

    DirectX::XMFLOAT3 p0, p1, p2;
    dx::XMVECTOR v0, v1, v2;
    for (UINT32 i = 0; i < indicesSize; i+=3) {
        //get a triangle 3 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p2 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+2] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x+pos.x,p0.y+pos.y,p0.z+pos.z,0.0f };
        v1 = { p1.x+pos.x,p1.y+pos.y,p1.z+pos.z,0.0f };
        v2 = { p2.x+pos.x,p2.y+pos.y,p2.z+pos.z,0.0f };

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
    transform.position = {pos.x,pos.y,pos.z,0.0f};
}

DirectX::XMFLOAT3 CollisionGeomerty::GetPos() noexcept
{
    return {XMVectorGetX(transform.position),XMVectorGetY(transform.position) ,XMVectorGetZ(transform.position) };
}

void CollisionGeomerty::SetColor(DirectX::XMFLOAT3 Color) noexcept
{
    this->color = Color;
}

void CollisionGeomerty::Bind(Graphics& gfx) noexcept
{
    DirectX::XMFLOAT3 dataCopy;
    if (Selected)dataCopy = { 0.8f * color.x,0.8f * color.y,0.8f * color.z, };
    else dataCopy = color;
    pCBufColor.Update(gfx, { dataCopy,0.0f });
    pCBufColor.Bind(gfx);
}

void CollisionGeomerty::SetSelect(bool IsSelected) noexcept
{
    Selected = IsSelected;
}

FTransform CollisionGeomerty::GetTransform()
{
    return transform;
}

void CollisionGeomerty::SetTransform(FTransform& transform)
{
    this->transform = transform;
}

void CollisionGeomerty::Draw(Graphics& gfx) const noexcept
{
}



Line::Line(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos, int lineWidth)
    :
    CollisionGeomerty(gfx,_vertexBuffer,_indices,_pos),
    lineWidth(lineWidth)
{
    using namespace Bind;
    AddBind(VertexBuffer::Resolve(gfx, "LineCollisionGeomerty", vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, "LineCollisionGeomerty", indices));
    AddBind(Bind::PixelShader::Resolve(gfx, "LinePS.cso"));

    auto pvs = Bind::VertexShader::Resolve(gfx, "LineVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), pvsb));

    AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

    struct PSColorConstant
    {
        DirectX::XMFLOAT3 color;
        float padding;
    } colorConst;
    colorConst.color = this->color;
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));

    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void Line::Bind(Graphics& gfx) noexcept
{
    DirectX::XMFLOAT3 dataCopy;
    if (Selected)dataCopy = { 0.8f * color.x,0.8f * color.y,0.8f * color.z, };
    else dataCopy = color;
    pCBufColor.Update(gfx, { dataCopy,0.0f });
    pCBufColor.Bind(gfx);
}

std::vector<CollisionGeomerty::CollisionRes> Line::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
    namespace dx = DirectX;

    std::vector<CollisionRes> result;
    UINT32 indicesSize = indices.size();
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
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = dx::XMVector3Normalize({ lineVector.x,lineVector.y,lineVector.z,0.0f });
    XMFLOAT3 pos;
    XMStoreFloat3(&pos, transform.position);

    DirectX::XMFLOAT3 p0, p1;
    dx::XMVECTOR v0, v1;

    for (UINT32 i = 0; i < indicesSize; i += 2) {
        //get a line 2 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i + 1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x + pos.x,p0.y + pos.y,p0.z + pos.z,0.0f };
        v1 = { p1.x + pos.x,p1.y + pos.y,p1.z + pos.z,0.0f };

        dx::XMVECTOR rayOrigin = dx::XMLoadFloat3(&lineBeginPos);
        dx::XMVECTOR rayDir = dx::XMLoadFloat3(&lineVector);
        dx::XMVECTOR segmentDir = v1 - v0;
        dx::XMVECTOR originToSegment = rayOrigin - v0;

        float a = dx::XMVector3Dot(segmentDir, segmentDir).m128_f32[0];
        float b = dx::XMVector3Dot(segmentDir, rayDir).m128_f32[0];
        float c = dx::XMVector3Dot(rayDir, rayDir).m128_f32[0];
        float d = dx::XMVector3Dot(segmentDir, originToSegment).m128_f32[0];
        float e = dx::XMVector3Dot(rayDir, originToSegment).m128_f32[0];

        float D = a * c - b * b;

        float t = -(a * e - b * d) / D;
        float s = (b * e - c * d) / D;
        s = std::clamp(s, 0.0f, 1.0f);

        dx::XMVECTOR pointOnRay = rayOrigin + t * rayDir;
        dx::XMVECTOR pointOnSegment = v0 + s * segmentDir;

        dx::XMVECTOR diff = pointOnRay - pointOnSegment;
        float distance = dx::XMVector3Length(diff).m128_f32[0];

        //hitted line?
        float minDistance=0.2f;
        if (distance < minDistance) {
            result.push_back({
                {
                    dx::XMVectorGetX(pointOnRay),
                    dx::XMVectorGetY(pointOnRay),
                    dx::XMVectorGetZ(pointOnRay),
                },
                {
                    dx::XMVectorGetX(pointOnSegment),
                    dx::XMVectorGetY(pointOnSegment),
                    dx::XMVectorGetZ(pointOnSegment),
                },{indices[i],indices[i + 1],UINT16_MAX},t
                });
        }
    }
    return result;
}

void Line::Draw(Graphics& gfx) const noexcept
{
    for (auto& b : binds)
    {
        b->Bind(gfx);
    }
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

TriangelGeo::TriangelGeo(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
    :
    CollisionGeomerty(gfx,_vertexBuffer,_indices,_pos)
{
    using namespace Bind;
    AddBind(VertexBuffer::Resolve(gfx, "CollisionGeomerty", vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, "CollisionGeomerty", indices));
    AddBind(Bind::PixelShader::Resolve(gfx, "CollisionGeomertyPS.cso"));

    auto pvs = Bind::VertexShader::Resolve(gfx, "CollisionGeomertyVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), pvsb));

    AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    struct PSColorConstant
    {
        DirectX::XMFLOAT3 color;
        float padding;
    } colorConst;
    colorConst.color = this->color;
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));

    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void TriangelGeo::Bind(Graphics& gfx) noexcept
{
    DirectX::XMFLOAT3 dataCopy;
    if (Selected)dataCopy = { 0.8f * color.x,0.8f * color.y,0.8f * color.z, };
    else dataCopy = color;
    pCBufColor.Update(gfx, { dataCopy,0.0f });
    pCBufColor.Bind(gfx);
}

void TriangelGeo::Draw(Graphics& gfx) const noexcept
{
    for (auto& b : binds)
    {
        b->Bind(gfx);
    }
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

Arrow::Arrow(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
    :CollisionGeomerty(gfx,_vertexBuffer,_indices,_pos)
{
    using namespace Bind;
    AddBind(VertexBuffer::Resolve(gfx, "Arrow", vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, "Arrow", indices));
    AddBind(Bind::PixelShader::Resolve(gfx, "ArrowPS.cso"));

    auto pvs = Bind::VertexShader::Resolve(gfx, "ArrowVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));
    auto a = vertexBuffer.GetLayout().GetD3DLayout();
    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer.GetLayout(), pvsb));

    AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    struct PSColorConstant
    {
        DirectX::XMFLOAT3 color;
        float padding;
    } colorConst;
    colorConst.color = this->color;
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));

    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

std::shared_ptr<Arrow> Arrow::ArrowConstruceHelper(Graphics& gfx, std::string filePath)
{
    DirectX::XMFLOAT3 color[] = {
        {1.0f,0.0f,0.0f},
        {0.0f,1.0f,0.0f},
        {0.0f,0.0f,1.0f}
    };
    DirectX::XMMATRIX RotateMatrix[] = {
        DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(0.0f,DirectX::XM_PIDIV2, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(0.0f,0.0f,DirectX::XM_PIDIV2),
    };
    Assimp::Importer imp;
    const auto pScene = imp.ReadFile(filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded
    );
    assert(pScene != nullptr);
    pScene->mNumMeshes;
    auto mesh = pScene->mMeshes[0];
    Dvtx::VertexBuffer vBuf(
        Dvtx::VertexLayout{}
        .Append(Dvtx::VertexLayout::Position3D)
        .Append(Dvtx::VertexLayout::Float3Color)
    );
    for (int i = 0; i < mesh->mNumVertices; i++) {
        vBuf.EmplaceBack(
            *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[i]),
            *reinterpret_cast<XMFLOAT3*>(&color[2])
        );
    }

    std::vector<unsigned short> indices;
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const auto& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    return std::make_shared<Arrow>(gfx,vBuf,indices);
}

void Arrow::Draw(Graphics& gfx) const noexcept
{
    for (auto& b : binds)
    {
        b->Bind(gfx);
    }
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}
