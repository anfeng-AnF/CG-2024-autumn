#include "CollisionGeometry.h"
#include "imguiManager.h"
#include "CtrlableGeometry.h"

CollisionGeometry::CollisionGeometry(Graphics& gfx, Dvtx::VertexBuffer &_vertexBuffer, std::vector<uint16_t> _indices,DirectX::XMFLOAT3 _pos):
    vertexBuffer(std::make_unique<Dvtx::VertexBuffer>(_vertexBuffer)),
    indices(_indices),
    transform(_pos),
    pCBufColor(gfx,3)
{

}

DirectX::XMMATRIX CollisionGeometry::GetTransformXM() const noexcept
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

std::vector<CollisionGeometry::CollisionRes> CollisionGeometry::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix, float posOffset)
{
    namespace dx=DirectX;

    UINT32 indicesSize =(UINT32) indices.size();
    auto data = vertexBuffer->GetData();
    std::vector<CollisionRes> result;

    //cacularte data size
    const auto layout = vertexBuffer->GetLayout().GetD3DLayout();
    const auto layoutSize = vertexBuffer->GetLayout().Size();
    UINT32 offset = 0;
    for (const auto& type : layout) {
        if (type.SemanticName == Dvtx::VertexLayout::Map<Dvtx::VertexLayout::Position3D>::semantic) {
            offset = type.AlignedByteOffset;
            break;
        }
    }
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = dx::XMVector3Normalize({ lineVector.x,lineVector.y,lineVector.z,0.0f });

    DirectX::XMFLOAT3 p0, p1, p2;
    dx::XMVECTOR v0, v1, v2;
    for (UINT32 i = 0; i < indicesSize; i+=3) {
        //get a triangle 3 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p2 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i+2] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x,p0.y,p0.z,1.0f };
        v1 = { p1.x,p1.y,p1.z,1.0f };
        v2 = { p2.x,p2.y,p2.z,1.0f };
        v0 = DirectX::XMVector3Transform(v0, transformMatrix);
        v1 = DirectX::XMVector3Transform(v1, transformMatrix);
        v2 = DirectX::XMVector3Transform(v2, transformMatrix);


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

void CollisionGeometry::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    transform.position = {pos.x,pos.y,pos.z,0.0f};
}

DirectX::XMFLOAT3 CollisionGeometry::GetPos() noexcept
{
    return {XMVectorGetX(transform.position),XMVectorGetY(transform.position) ,XMVectorGetZ(transform.position) };
}

void CollisionGeometry::SetColor(DirectX::XMFLOAT3 Color) noexcept
{
    this->color = Color;
}

DirectX::XMFLOAT3 CollisionGeometry::GetColor() noexcept
{
    return color;
}

void CollisionGeometry::Bind(Graphics& gfx) noexcept
{
    DirectX::XMFLOAT3 dataCopy;
    if (Selected)dataCopy = { 0.8f * color.x,0.8f * color.y,0.8f * color.z, };
    else dataCopy = color;
    pCBufColor.Update(gfx, { dataCopy,0.0f });
    pCBufColor.Bind(gfx);
}

void CollisionGeometry::SetSelect(bool IsSelected) noexcept
{
    Selected = IsSelected;
}

bool CollisionGeometry::GetSelect() noexcept
{
    return Selected;
}

FTransform CollisionGeometry::GetTransform()
{
    return transform;
}

void CollisionGeometry::SetTransform(FTransform& transform)
{
    this->transform = transform;
}

void CollisionGeometry::Draw(Graphics& gfx) const noexcept
{
}



Line::Line(Graphics& gfx,Camera&cam, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
    :
    CollisionGeometry(gfx,_vertexBuffer,_indices,_pos),
    cam(& cam),
    gfx(gfx)
{
    using namespace Bind;
    static int id = 0;
    AddBind(VertexBuffer::Resolve(gfx, "LineCollisionGeomerty"+std::to_string(id++), *vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, "LineCollisionGeomerty" + std::to_string(id), indices));
    AddBind(Bind::PixelShader::Resolve(gfx, "LinePS.cso"));

    auto pvs = Bind::VertexShader::Resolve(gfx, "LineVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer->GetLayout(), pvsb));

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

std::vector<CollisionGeometry::CollisionRes> Line::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector,DirectX::XMMATRIX transformMatrix, float posOffset)
{
    namespace dx = DirectX;

    std::vector<CollisionRes> result;
    UINT32 indicesSize = (UINT32)indices.size();
    auto data = vertexBuffer->GetData();

    //cacularte data size
    const auto layout = vertexBuffer->GetLayout().GetD3DLayout();
    const auto layoutSize = vertexBuffer->GetLayout().Size();
    UINT32 offset = 0;
    for (const auto& type : layout) {
        if (type.SemanticName == Dvtx::VertexLayout::Map<Dvtx::VertexLayout::Position3D>::semantic) {
            offset = type.AlignedByteOffset;
            break;
        }
    }
    const dx::XMVECTOR lineBeginV = { lineBeginPos.x ,lineBeginPos.y,lineBeginPos.z,0.0f };
    const dx::XMVECTOR lineDirection = dx::XMVector3Normalize({ lineVector.x,lineVector.y,lineVector.z,0.0f });

    DirectX::XMFLOAT3 p0, p1;
    dx::XMVECTOR v0, v1;

    for (UINT32 i = 0; i < indicesSize; i += 2) {
        //get a line 2 points world pos
        p0 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        p1 = *reinterpret_cast<DirectX::XMFLOAT3*>(getSubStr(data + indices[i + 1] * layoutSize + offset, sizeof(DirectX::XMFLOAT3)).data());
        v0 = { p0.x,p0.y,p0.z,1.0f };
        v1 = { p1.x,p1.y,p1.z,1.0f };
        v0 = XMVector3Transform(v0, transformMatrix);
        v1 = XMVector3Transform(v1, transformMatrix); 


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

        //hitted line? todo:use dynamic distance

        XMVECTOR objectPosition = transform.position;
        XMMATRIX viewMatrix = cam->GetMatrix();
        XMMATRIX projectionMatrix = gfx.GetProjection();
        XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

        XMVECTOR pos4 = XMVectorSetW(objectPosition, 1.0f);
        XMVECTOR clipPos = XMVector4Transform(pos4, viewProjectionMatrix);

        float depth = clipPos.m128_f32[3];

        // 计算缩放因子
        float referenceDepth = 20.0f;
        
        float dynamicMinDistance= depth / referenceDepth+ posOffset;
        if (distance < dynamicMinDistance) {
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
    CollisionGeometry(gfx,_vertexBuffer,_indices,_pos)
{
    using namespace Bind;
    AddBind(VertexBuffer::Resolve(gfx, "CollisionGeomerty", *vertexBuffer));
    AddBind(IndexBuffer::Resolve(gfx, "CollisionGeomerty", indices));
    AddBind(Bind::PixelShader::Resolve(gfx, "CollisionGeomertyPS.cso"));

    auto pvs = Bind::VertexShader::Resolve(gfx, "CollisionGeomertyVS.cso");
    auto pvsb = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(Bind::InputLayout::Resolve(gfx, vertexBuffer->GetLayout(), pvsb));

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

WidthLine::WidthLine(Graphics& gfx, Camera& cam, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos, float lineWidth)
    :
    Line(gfx,cam,_vertexBuffer,_indices,_pos),
    width(lineWidth),
    gcBuf(gfx)
{
    auto it=std::find(binds.begin(), binds.end(), Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));
    if(it!=binds.end())
        *it = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ);
    //Convert linelist_adj method line to linelist for collision detection
    std::vector<uint16_t> ind;
    for (auto i = 0u; i < _indices.size(); i+=4) {
        ind.push_back(_indices[i + 1]);
        ind.push_back(_indices[i + 2]);
    }
    this->indices = ind;
}

std::vector<CollisionGeometry::CollisionRes> WidthLine::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix, float posOffset)
{
    return __super::TraceByLine(lineBeginPos,lineVector,transformMatrix,width);
}

void WidthLine::Draw(Graphics& gfx) const noexcept
{
    auto GS = Bind::GeometryShader::Resolve(gfx, "LineWidthGS.cso");
    for (auto& b : binds)
    {
        b->Bind(gfx);
    }
    //if (width > 0.02f);
    GS->Bind(gfx);
    gfx.DrawIndexed(pIndexBuffer->GetCount());
    GS->UnBind(gfx);
}

void WidthLine::Bind(Graphics& gfx) noexcept
{
    CbufData data = {
        DirectX::XMMatrixTranspose(GetTransformXM()*gfx.GetCamera() * gfx.GetProjection()),
        DirectX::XMMatrixTranspose(GetTransformXM() * gfx.GetCamera()),
        width
    };
    gcBuf.Update(gfx, data);
    gcBuf.Bind(gfx);
}

//Distance from a point to a line-----distance from the nearest point to the start of the ray
std::pair<float,float> DistanceFromPointToLine(const XMFLOAT3& lineBeginPos, const XMFLOAT3& lineVector, const XMVECTOR& pointPosition) {
    XMVECTOR lineStart = XMLoadFloat3(&lineBeginPos);
    XMVECTOR lineDir = XMLoadFloat3(&lineVector);

    XMVECTOR pointToLineStart = pointPosition - lineStart;

    XMVECTOR lineUnitDir = XMVector3Normalize(lineDir);

    float projectionLength = XMVectorGetX(XMVector3Dot(pointToLineStart, lineUnitDir));
    XMVECTOR projection = lineStart + projectionLength * lineUnitDir;

    XMVECTOR pointToProjection = pointPosition - projection;

    return { XMVectorGetX(XMVector3Length(pointToProjection)),projectionLength };
}

BezierLine::BezierLine(Graphics& gfx,Camera& cam, CollisionGeoManager* CGM)
    :
    CollisionGeometry(gfx),
    point(gfx, XMFLOAT3{ 0.8f,0.2f,0.2f }, XMFLOAT3{0,0,0}, 0.1f),
    cam(cam),
    gfx(gfx),
    CGM(CGM)
{
    FTransform point;
    ctrlPoint.push_back(std::make_shared<ControlPoint>(gfx, XMFLOAT3{ 1.0f, 0.0f, 1.0f }));
    ctrlPoint.push_back(std::make_shared<ControlPoint>(gfx, XMFLOAT3{ -1.0f, 0.0f, -1.0f }));
    for (auto c : ctrlPoint) {
        CGM->AddGeometry(c);
    }
    ReGenerateBezier();
}


void BezierLine::Draw(Graphics& gfx) const noexcept
{
    if (Bezier) {
        Bezier->Bind(gfx);
        Bezier->Draw(gfx);
    }
    if (Selected) {
        ImGui::Begin("Bessel curve information");
        if (ImGui::Button("Add Control Point")) {
            AddControlPoint();
        }        
        if (ImGui::Button("Delete control point")) {
            AddControlPoint();
        }
        if (ImGui::SliderFloat("Width:",&Bezier->width, 0.0f, 100.0f)) {
            Width = Bezier->width;
        }
        ImGui::End();
    }
}

void BezierLine::Bind(Graphics& gfx) noexcept
{
    for (auto& cPoint : ctrlPoint) {
        if (cPoint->bMoved) {
            ReGenerateBezier();
        }
        if (cPoint->GetSelect()) {
            Selected = true;
        }
    }
}

std::vector<CollisionGeometry::CollisionRes> BezierLine::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix, float posOffset)
{
    return {};
}

void BezierLine::SetTransform(FTransform& transform)
{

}

void BezierLine::AddControlPoint() const
{
    auto back = std::make_shared<ControlPoint>(gfx, XMFLOAT3{ 0.0f,0.0f,0.0f });
    ctrlPoint.push_back(back);
    CGM->AddGeometry(back);
    ReGenerateBezier();
}

void BezierLine::DeleteControlPoint() const
{
    for (auto c = ctrlPoint.begin(); c != ctrlPoint.end(); c++)
    {
        if (ctrlPoint.size() < 2) {
            msg = "Bessel curve control points cannot be less than 2";
            return;
        }
        if ((*c)->GetSelect()) {
            CGM->DeleteGeometry((*c));
            ctrlPoint.erase(c);
        }
    }
}


//std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> GenerationBezierLineData(std::vector<FTransform>points, int segment = 20) {
//    Dvtx::VertexBuffer vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
//    std::vector<uint16_t> indices;
//    for (int i = 0; i < points.size()-1; i++) {
//        indices.push_back(i - 1);
//        indices.push_back(i + 0);
//        indices.push_back(i + 1);
//        indices.push_back(i + 2);
//        vbuf.EmplaceBack(*reinterpret_cast<XMFLOAT3*>(points[i].position.m128_f32));
//    }
//    vbuf.EmplaceBack(*reinterpret_cast<XMFLOAT3*>(points[points.size()-1].position.m128_f32));
//    indices[0] = 0;
//    indices[indices.size() - 1] = points.size() - 1;
//    return { vbuf,indices };
//};
// 计算组合数的迭代方法
int binomialCoefficient(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;

    // 使用循环来计算组合数
    int result = 1;
    for (int i = 0; i < k; ++i) {
        result = result * (n - i) / (i + 1);
    }
    return result;
}

// 计算贝塞尔曲线上的点
DirectX::XMFLOAT3 bezier(const std::vector<FTransform>& points, int n, float t) {
    DirectX::XMFLOAT3 mypoint = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i <= n; i++) {
        float coefficient = binomialCoefficient(n, i) * pow(1 - t, n - i) * pow(t, i);
        mypoint.x += coefficient * DirectX::XMVectorGetX(points[i].position);
        mypoint.y += coefficient * DirectX::XMVectorGetY(points[i].position); // 改为 GetY 以符合 3D
        mypoint.z += coefficient * DirectX::XMVectorGetZ(points[i].position); // 改为 GetZ 以符合 3D
    }

    return mypoint;
}

std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> GenerationBezierLineData(std::vector<FTransform>points, int segment = 20) {
    //预处理输入顶点数组
    std::vector<FTransform> GenerateBezierPoints((points.size() - 1) * 3+1);
    int size = (points.size() - 1) * 3 + 1;
    //第一段：
    GenerateBezierPoints[0] = points[0];
    GenerateBezierPoints[1].position = (-points[0].GetRightVector() * XMVector3Length(points[0].scale)) + points[0].position;
    for (int i = 1; i < points.size() - 1; i++) {
        GenerateBezierPoints[i * 3] = points[i];
        GenerateBezierPoints[i * 3 + 1].position = ((-points[i].GetRightVector()) * XMVector3Length(points[i].scale)) + points[i].position;
        GenerateBezierPoints[i * 3 - 1].position = ((points[i].GetRightVector()) * XMVector3Length(points[i].scale)) + points[i].position;
    }

    // 最后一段
    GenerateBezierPoints[size - 1] = points[points.size() - 1];
    GenerateBezierPoints[size - 2].position = (points[points.size() - 1].GetRightVector())*XMVector3Length(points[points.size() - 1].scale) + points[points.size() - 1].position;


    Dvtx::VertexBuffer vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
    std::vector<uint16_t> indices;
    int n = static_cast<int>(GenerateBezierPoints.size()) - 1;
    int numPoints = segment * n + 1;
    //OutputDebugString(L"hello");
    for (int i = 0; i < numPoints; i++) {
        float t = (float)i / (float)numPoints;
        vbuf.EmplaceBack(bezier(GenerateBezierPoints, n, t));
    }

    for (int i = 0; i < numPoints - 1; i++) {
        indices.push_back(i - 1);
        indices.push_back(i + 0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }
    indices[0] = 0;
    indices[indices.size() - 1] = GenerateBezierPoints.size() - 1;

    return { vbuf,indices };
};
void BezierLine::ReGenerateBezier()const
{
    std::vector<FTransform> cpoint;
    for (auto c : ctrlPoint) {
        cpoint.push_back(c->GetTransform());
    }
    auto [vbuf, ind] = GenerationBezierLineData(cpoint);
    this->Bezier = std::make_unique<WidthLine>(gfx, cam, vbuf, ind, XMFLOAT3{ 0.0f,0.0f,0.0f } ,Width);
}

ControlPoint::ControlPoint(Graphics& gfx, XMFLOAT3 pos)
    :
    point(gfx, XMFLOAT3{ 1.0f,1.0f,1.0f }, pos, 0.05f),
    CollisionGeometry(gfx)
{
    transform = FTransform();
    this->transform.position = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);
}

void ControlPoint::Draw(Graphics& gfx) const noexcept
{
    point.Draw(gfx);
}

void ControlPoint::Bind(Graphics& gfx) noexcept
{
}

std::vector<CollisionGeometry::CollisionRes> ControlPoint::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix, float posOffset)
{
    auto [lengthP,lengthLine] = DistanceFromPointToLine(lineBeginPos, lineVector, transform.position);
    CollisionRes cRes;
    if (lengthP < lengthLine*0.1) {
        cRes.hitDistance = lengthLine;
        return { cRes };
    }
    return {};
}

void ControlPoint::SetTransform(FTransform& transform)
{
    this->transform = transform;
    XMFLOAT3 f3;
    XMStoreFloat3(&f3, transform.position);
    point.SetPos(f3);
    bMoved = true;
}
