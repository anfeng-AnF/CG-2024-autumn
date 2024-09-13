#include "CtrlableGeometry.h"

TransformComponentBase::TransformComponentBase(Graphics& gfx, Camera& cam, std::string filePath) :
	DGM(DebugGraphsMannger::GetInstence()),
    gfx(gfx),
    cam(cam)
{
    pTransformCtrlComponent = std::make_unique<TransformCtrlComponent>(gfx, cam, filePath);
}

void TransformComponentBase::draw(Graphics& gfx) const noexcept
{
    pTransformCtrlComponent->SetTransform(transform);
    auto depth = Bind::DepthStencilState(gfx);
    depth.Bind(gfx);
    pTransformCtrlComponent->Draw(gfx);
    depth.UnBind(gfx);
}

void TransformComponentBase::SetTransform(FTransform transform)
{
    this->transform = transform;
}

bool TransformComponentBase::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
	auto res = pTransformCtrlComponent->TraceByLineGetNearestMesh(lineBeginPos, lineVector);
	if (res.second) {
		if (res.second->GetName() == "X") tAxis = X;
		if (res.second->GetName() == "Y") tAxis = Y;
		if (res.second->GetName() == "Z") tAxis = Z;
		if (res.second->GetName() == "XY")tAxis = XY;
		if (res.second->GetName() == "XZ")tAxis = XZ;
		if (res.second->GetName() == "YZ")tAxis = YZ;
		if (res.second->GetName() =="XYZ")tAxis = XYZ;

		DGM.AddGeo(std::make_shared<DebugSphere>(gfx, XMFLOAT3{ 0.7f,0.1f,0.1f }, res.first.pos));
		res.second->SetSelect(true);
		ctrlingMesh = res.second;
		return true;
	}
	else
	{
		tAxis = NONE_AXIS;
	}
	return false;
}

void TransformComponentBase::EndTransform()
{
	if (ctrlingMesh) {
		ctrlingMesh->SetSelect(false);
		ctrlingMesh = nullptr;
        onTransform = false;
	}
}

LineRay TransformComponentBase::GetPlaneFlatPointNormal(LineRay line, XMVECTOR point)
{
    XMVECTOR lineOrigin = XMLoadFloat3(&line.rayOrigin);
    XMVECTOR lineDirection = XMLoadFloat3(&line.rayDirection);
    XMVECTOR targetPoint = point;

    XMVECTOR vectorOP = XMVectorSubtract(targetPoint, lineOrigin);

    float t = XMVector3Dot(vectorOP, lineDirection).m128_f32[0] / XMVector3Dot(lineDirection, lineDirection).m128_f32[0];
    XMVECTOR closestPoint = XMVectorAdd(lineOrigin, XMVectorScale(lineDirection, t));

    XMVECTOR vectorToTarget = XMVectorSubtract(targetPoint, closestPoint);
    vectorToTarget = XMVector3Normalize(vectorToTarget);
    LineRay result;
    XMStoreFloat3(&result.rayOrigin, closestPoint);  
    XMStoreFloat3(&result.rayDirection, vectorToTarget);

    return result;
}

XMFLOAT3 TransformComponentBase::GetIntersectionPlaneLine(LineRay plane, LineRay line)
{
    XMVECTOR planePoint = XMLoadFloat3(&plane.rayOrigin);
    XMVECTOR planeNormal = XMLoadFloat3(&plane.rayDirection);
    XMVECTOR lineOrigin = XMLoadFloat3(&line.rayOrigin);
    XMVECTOR lineDirection = XMLoadFloat3(&line.rayDirection);

    XMVECTOR numerator = XMVector3Dot(XMVectorSubtract(planePoint, lineOrigin), planeNormal);
    XMVECTOR denominator = XMVector3Dot(lineDirection, planeNormal);

    if (XMVector3Equal(denominator, XMVectorZero()))
    {
        throw std::runtime_error("The line is parallel to the plane or lies within the plane.");
    }

    float t = numerator.m128_f32[0] / denominator.m128_f32[0];

    XMVECTOR intersection = XMVectorAdd(lineOrigin, XMVectorScale(lineDirection, t));

    XMFLOAT3 intersectionPoint;
    XMStoreFloat3(&intersectionPoint, intersection);

    return intersectionPoint;
}

float TransformComponentBase::GetProjectionLength(const XMFLOAT3& aDirection, const XMFLOAT3& bDirection)
{
    XMVECTOR aVec = XMLoadFloat3(&aDirection);
    XMVECTOR bVec = XMLoadFloat3(&bDirection);

    XMVECTOR dotProduct = XMVector3Dot(aVec, bVec);

    XMVECTOR bLength = XMVector3Length(bVec);

    float projectionLength = dotProduct.m128_f32[0] / bLength.m128_f32[0];

    return projectionLength;
}
float TransformComponentBase::GetProjectionLength(const XMVECTOR& aDirection, const XMVECTOR& bDirection)
{
    XMVECTOR dotProduct = XMVector3Dot(aDirection, bDirection);

    XMVECTOR bLength = XMVector3Length(bDirection);

    float projectionLength = dotProduct.m128_f32[0] / bLength.m128_f32[0];

    return projectionLength;
}

XMMATRIX TransformComponentBase::CreateTranslationMatrix(const XMFLOAT3& translation)
{
    return XMMatrixTranslation(translation.x, translation.y, translation.z);
}

LineRay TransformComponentBase::GetPlane()
{
    //get plane
    LineRay plane;
    switch (tAxis)
    {
    case TransformComponentBase::X:
        plane = GetPlaneFlatPointNormal(LineRay{ transform.position,transform.GetRightVector() }, cam.GetTransform().position);
        break;
    case TransformComponentBase::Y:
        plane = GetPlaneFlatPointNormal(LineRay{ transform.position,transform.GetUpVector() }, cam.GetTransform().position);
        break;
    case TransformComponentBase::Z:
        plane = GetPlaneFlatPointNormal(LineRay{ transform.position,transform.GetForwardVector() }, cam.GetTransform().position);
        break;
    case TransformComponentBase::XY:
        plane = { transform.position,XMVector3Normalize(XMVector3Cross(transform.GetRightVector(),transform.GetUpVector())) };
        break;
    case TransformComponentBase::XZ:
        plane = { transform.position,XMVector3Normalize(XMVector3Cross(transform.GetRightVector(),transform.GetForwardVector())) };
        break;
    case TransformComponentBase::YZ:
        plane = { transform.position,XMVector3Normalize(XMVector3Cross(transform.GetUpVector(),transform.GetForwardVector())) };
        break;
    case TransformComponentBase::XYZ:
        plane = { transform.position,XMVector3Normalize(cam.GetTransform().GetForwardVector()) };
        break;
    default:
        throw std::runtime_error("How did you get here without the center shaft?");
        break;
    }
    return plane;
}

XMVECTOR TransformComponentBase::XM3F2XMVEC(XMFLOAT3 f3)
{
    return XMVectorSet(f3.x,f3.y,f3.z,0.0f);
}

TranslateComponent::TranslateComponent(Graphics& gfx, Camera& cam, std::string filePath)
    :
    TransformComponentBase(gfx, cam, filePath)
{
}

XMMATRIX TranslateComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
    //get plane
    static LineRay plane;

    if (!onTransform) {
        plane = GetPlane();
        fromWorldPos= this->GetIntersectionPlaneLine(plane, LineRay(from, wnd, cam));
        onTransform = true;
    }
    auto toWorldPos = this->GetIntersectionPlaneLine(plane, LineRay(to+from, wnd, cam));
    //Preparation data completed//

    XMMATRIX resMatrix = XMMatrixIdentity();
    XMVECTOR translateDirection;
    float t=1.0f;
    switch (tAxis)
    {
    case TransformComponentBase::X:
        translateDirection = transform.GetRightVector();
        t *= this->GetProjectionLength(toWorldPos-fromWorldPos, { translateDirection.m128_f32[0],translateDirection.m128_f32[1],translateDirection.m128_f32[2] });
        break;
    case TransformComponentBase::Y:
        translateDirection = transform.GetUpVector();
        t *= this->GetProjectionLength(toWorldPos - fromWorldPos, { translateDirection.m128_f32[0],translateDirection.m128_f32[1],translateDirection.m128_f32[2] });
        break;
    case TransformComponentBase::Z:
        translateDirection = transform.GetForwardVector();
        t *= this->GetProjectionLength(toWorldPos - fromWorldPos, { translateDirection.m128_f32[0],translateDirection.m128_f32[1],translateDirection.m128_f32[2] });
        break;
    case TransformComponentBase::XY:
    case TransformComponentBase::YZ:
    case TransformComponentBase::XZ:
    case TransformComponentBase::XYZ:
        XMFLOAT3 a = toWorldPos - fromWorldPos;
        translateDirection =XMVectorSet(a.x, a.y, a.z, 0.0f);
        break;
    }
    resMatrix = this->CreateTranslationMatrix({ translateDirection.m128_f32[0] * t, translateDirection.m128_f32[1] * t, translateDirection.m128_f32[2] * t });
	return resMatrix;
}

RotationComponent::RotationComponent(Graphics& gfx, Camera& cam, std::string filePath)
    :
    TransformComponentBase(gfx, cam, filePath) 
{
}

XMMATRIX RotationComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
    static LineRay plane;
    static XMMATRIX rotationMatrix = XMMatrixIdentity();
    static screenPos lastPosTo = from;
    screenPos deltaTo = {};
    if (!onTransform) {
        plane = GetPlane();
        onTransform = true;
        rotationMatrix = XMMatrixIdentity();
        deltaTo = to + from - lastPosTo;
    }
    beginDirection = XMVector3Normalize(XMVectorSubtract(
        XM3F2XMVEC(GetIntersectionPlaneLine(plane, LineRay(from + lastPosTo, wnd, cam))),
        transform.position));
    deltaTo = to - lastPosTo;
    lastPosTo = to;
    if (!(deltaTo != screenPos{ 0,0 }))return rotationMatrix;
    XMVECTOR toDirection = XMVector3Normalize(XMVectorSubtract(
        XM3F2XMVEC(GetIntersectionPlaneLine(plane, LineRay(from + lastPosTo + deltaTo, wnd, cam))),
        transform.position));

    XMVECTOR axis;
    switch (tAxis)
    {
    case TransformComponentBase::XY:
        axis = transform.GetForwardVector();
        break;
    case TransformComponentBase::XZ:
        axis = transform.GetUpVector();
        break;
    case TransformComponentBase::YZ:
        axis = transform.GetRightVector();
        break;
    }
    float angle = acos(XMVectorGetX(XMVector3Dot(beginDirection, toDirection)));
    //Determine the direction
    float dir =XMVector3Dot(XMVector3Cross(beginDirection, toDirection),axis).m128_f32[0];
    if (dir < 0)angle *= -1;

    XMMATRIX rotationMatrixDelta = XMMatrixRotationAxis(axis, angle);
    rotationMatrix = XMMatrixMultiply(rotationMatrix, rotationMatrixDelta);

	return rotationMatrix;
}

ScaleComponent::ScaleComponent(Graphics& gfx, Camera& cam, std::string filePath)
    :
    TransformComponentBase(gfx, cam, filePath)
{
}

XMMATRIX ScaleComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
    static LineRay plane = {};
    if (!onTransform) {
        onTransform = true;
        plane = GetPlane();
        BeginScaleLength = GetScaleLength(plane,LineRay{from,wnd,cam});
        switch (tAxis)
        {
        case TransformComponentBase::X:
            XMStoreFloat3(&scaleDirection,transform.GetRightVector());
            break;
        case TransformComponentBase::Y:
            XMStoreFloat3(&scaleDirection, transform.GetUpVector());
            break;
        case TransformComponentBase::Z:
            XMStoreFloat3(&scaleDirection, transform.GetForwardVector());
            break;
        case TransformComponentBase::XY:
        case TransformComponentBase::XZ:
        case TransformComponentBase::YZ:
            XMStoreFloat3(&scaleDirection, XMVector3Normalize(
                XMVectorSubtract(
                    XM3F2XMVEC(GetIntersectionPlaneLine(plane, LineRay(from, wnd, cam))),
                    transform.position)));
            break;
        case TransformComponentBase::XYZ:
            XMStoreFloat3(&scaleDirection, XMVector3Normalize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)));
            break;
        }
    }
    float toScaleLength = GetScaleLength(plane, LineRay{to+from,wnd,cam});
    float scaleFactor =abs(toScaleLength) / BeginScaleLength-1.0f;
    return XMMatrixScaling(scaleDirection.x * scaleFactor + 1.0f, scaleDirection.y * scaleFactor + 1.0f, scaleDirection.z * scaleFactor + 1.0f);
}
#define SET_LENGTH_PROJECTION(NORMALIZE_VEC) length = GetProjectionLength(\
XMVectorSubtract(XM3F2XMVEC(GetIntersectionPlaneLine(Plane, ray)), \
transform.position), NORMALIZE_VEC)
float ScaleComponent::GetScaleLength(LineRay Plane,LineRay ray)
{
    float length;
    switch (tAxis)
    {
    case TransformComponentBase::X:
        SET_LENGTH_PROJECTION(transform.GetRightVector());
           break;
    case TransformComponentBase::Y:
        SET_LENGTH_PROJECTION(transform.GetUpVector());
        break;        
    case TransformComponentBase::Z:
        SET_LENGTH_PROJECTION(transform.GetForwardVector());
        break;
    case TransformComponentBase::XY:
    case TransformComponentBase::XZ:
    case TransformComponentBase::YZ:
    case TransformComponentBase::XYZ:
        length = XMVector3Length(
            XMVectorSubtract(
                XM3F2XMVEC(GetIntersectionPlaneLine(Plane, ray)),
                transform.position
            )
        ).m128_f32[0];
        break;
    }
    return length;
}
#undef SET_LENGTH_PROJECTION(NORMALIZE_VEC) length = GetProjectionLength(\
XMVectorSubtract(XM3F2XMVEC(GetIntersectionPlaneLine(Plane, ray)), \
transform.position), NORMALIZE_VEC)
CollisionGeoManager::CollisionGeoManager(Window& wnd, Camera& cam) 
    :
    inputState(wnd, *this),
    gfx(&wnd.Gfx()), cam(&cam),
    DGM(DebugGraphsMannger::GetInstence())
{
    pTranslateComponent = std::make_unique<TranslateComponent>(wnd.Gfx(), cam, "Models\\Component\\Position.fbx");
    pRotationComponent = std::make_unique<RotationComponent>(wnd.Gfx(), cam, "Models\\Component\\Rotation.fbx");
    pScaleComponent = std::make_unique<ScaleComponent>(wnd.Gfx(),cam,"Models\\Component\\Scale.fbx");
}; 

void CollisionGeoManager::AddGeometry(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
    Geomertys[std::make_shared<CollisionGeometry>(gfx, _vertexBuffer, _indices, _pos)] = false;
}

void CollisionGeoManager::AddGeometry(std::shared_ptr<CollisionGeometry> Geo, bool isSelected)
{
    if (isSelected) {
        SelectedGeomertys.push_back({ Geo,Geo->GetTransform() });
        RenewOriginPointPos();
    }
    Geomertys[std::move(Geo)] = isSelected;
}

void CollisionGeoManager::TransformGeometryByImGui(Window& wnd)
{
    auto q=DirectX::XMQuaternionRotationRollPitchYaw(
        TransformData.DeltaRotationEuler.x / 180 * XM_PI,
        TransformData.DeltaRotationEuler.y / 180 * XM_PI,
        TransformData.DeltaRotationEuler.z / 180 * XM_PI
    );
    TransformData.deltaTransform.rotation = q;
}

void CollisionGeoManager::TransformGeometryByComponent(Window& wnd, screenPos Delta)
{
    XMMATRIX deltaTransform=XMMatrixIdentity();
    TransformData.deltaScreenPos +=Delta;
    switch (TransformData.transformationMethod)
    {
    case 0:
        assert(0 && L"How do you get this value when using ,amazing ~ ♪ : TransformData.transformationMethod");
        break;
    case 1:
        deltaTransform= pTranslateComponent->GetDeltaTransform(TransformData.beginScreenPos, TransformData.deltaScreenPos,wnd);
        break;
    case 2:
        deltaTransform = pScaleComponent->GetDeltaTransform(TransformData.beginScreenPos, TransformData.deltaScreenPos, wnd);
        break;
    case 3:
        deltaTransform = pRotationComponent->GetDeltaTransform(TransformData.beginScreenPos, TransformData.deltaScreenPos, wnd);
        break;
    default:
        assert(0 && "unknown value : TransformData.transformationMethod");
        break;
    }
    TransformData.deltaTransform = FTransform(deltaTransform);
    TransformData.DeltaRotationEuler = TransformData.deltaTransform.GetRotationEuler();
}

int CollisionGeoManager::SelectGeometry(screenPos pos, Window& wnd)
{
    LineRay ray(pos, wnd, *cam);
    XMFLOAT3 endPos = { ray.rayOrigin.x + ray.rayDirection.x * 1e4f, ray.rayOrigin.y + ray.rayDirection.y * 1e4f, ray.rayOrigin.z + ray.rayDirection.z * 1e4f };
    DGM.AddGeo(std::make_shared<DebugLine>(*gfx, ray.rayOrigin, endPos, XMFLOAT3(1.0f, 0.0f, 0.0f)));
    //0-none  1-translate  2-scale 3-rotation
    bool selectedComponent = false;
    if (SelectedGeomertys.size()) {
        switch (TransformData.transformationMethod)
        {
        case 1:
            selectedComponent = pTranslateComponent->TraceByLine(ray.rayOrigin, ray.rayDirection);
            break;
        case 2:
            selectedComponent = pScaleComponent->TraceByLine(ray.rayOrigin, ray.rayDirection);
            break;
        case 3:
            selectedComponent = pRotationComponent->TraceByLine(ray.rayOrigin, ray.rayDirection);
            break;
        default:
            break;
        }
    }
    if (selectedComponent)
    {
        this->TransformData.beginScreenPos = pos;
        this->TransformData.deltaScreenPos = { 0,0 };
        //return 2 for transform component be selected
        return 2;
    }

    //get the nearest geo
    float minDistance = D3D11_FLOAT32_MAX;
    std::shared_ptr<CollisionGeometry> nearestGeo = nullptr;
    for (auto& geo : Geomertys) {
        auto hitRes = geo.first->TraceByLine(ray.rayOrigin, ray.rayDirection, geo.first->GetTransformXM());
        if (hitRes.size()) {
            for (const auto& collisionRes : hitRes) {
                if (minDistance > collisionRes.hitDistance) {
                    minDistance = collisionRes.hitDistance;
                    nearestGeo = geo.first;
                }
                DGM.AddGeo(std::make_shared<DebugSphere>(*gfx, XMFLOAT3{ 0.8f,0.2f,0.2f }, collisionRes.pos, 0.02f));
            }
        }
    }
    RenewTransformDelta();
    //0 for nothing selected
    if (!nearestGeo) {
        //If click blank, remove all selected geometry
        SelectedGeomertys.clear();
        for (auto& geo : Geomertys) {
            geo.second = false;
            geo.first->SetSelect(false);
        }
        return 0;
    }

    //push the nearest hitted geo to selectedGeo list
    if (Geomertys[nearestGeo]) {
        SelectedGeomertys.remove({ nearestGeo,nearestGeo->GetTransform() });
        nearestGeo->SetSelect(false);
        Geomertys[nearestGeo] = false;
        RenewOriginPointPos();
    }
    else
    {
        auto transform = nearestGeo->GetTransform();
        Geomertys[nearestGeo] = true;
        nearestGeo->SetSelect(true);
        SelectedGeomertys.push_back({ std::move(nearestGeo),transform });
        RenewOriginPointPos();

    }
    if (SelectedGeomertys.size() == 1) {
        TransformData.color = SelectedGeomertys.front().first->GetColor();
    }
    // 1 for select collisionGeometry
    RenewOriginPointPos();
    return 1;
}

void CollisionGeoManager::ChangeTransformationMethod(TransformationMethod method)
{
    this->TransformData.transformationMethod = method;
}

void CollisionGeoManager::Draw(Graphics& gfx)
{
    for (const auto& obj : Geomertys) {
        if (auto a = static_cast<CollisionGeometry*>(obj.first.get())) {
            a->Bind(gfx);
        }
        obj.first->Draw(gfx);
    }
    if (SelectedGeomertys.size() == 0)return;
    switch (TransformData.transformationMethod)
    {
    case TransformationMethod::NONE:
        break;
    case TransformationMethod::TRANSLATE:
        pTranslateComponent->draw(gfx);
        break;
    case TransformationMethod::SCALE:
        pScaleComponent->draw(gfx);
        break;
    case TransformationMethod::ROTATION:
        pRotationComponent->draw(gfx);
        break;
    default:
        break;
    }
}

int CollisionGeoManager::GetSelectedGeoNum()
{
    return (int)SelectedGeomertys.size();
}

void CollisionGeoManager::Transform()
{
    auto axisTransformMatrix = TransformData.originTransform.GetMatrix();
    auto invAxisTrans = XMMatrixInverse(nullptr, axisTransformMatrix);

	for (auto val : SelectedGeomertys) {

		auto transform =FTransform(
            val.second.GetMatrix()*
            invAxisTrans*
            TransformData.deltaTransform.GetMatrix()*
            axisTransformMatrix
            );
		val.first->SetTransform(transform);
        if (TransformData.color.x != -1.0f) {
            val.first->SetColor(TransformData.color);
        }
	}
    pTranslateComponent->SetTransform(TransformData.originTransform + TransformData.deltaTransform);
    pScaleComponent->SetTransform(TransformData.originTransform + TransformData.deltaTransform);
    pRotationComponent->SetTransform(TransformData.originTransform + TransformData.deltaTransform);
}

void CollisionGeoManager::EndComponentTransform()
{
    switch (TransformData.transformationMethod)
    {
    case TransformationMethod::NONE:
        break;
    case TransformationMethod::TRANSLATE:
        pTranslateComponent->EndTransform();
        break;
    case TransformationMethod::SCALE:
        pScaleComponent->EndTransform();
        break;
    case TransformationMethod::ROTATION:
        pRotationComponent->EndTransform();
        break;
    default:
        break;
    }
}

void CollisionGeoManager::DrawImGui(Graphics& gfx)
{
    ImGui::BeginChild("transform", { 0,0 });
    ImGui::Checkbox("Automatically applying transformations?", &TransformData.autoApplyTransform);
    if (SelectedGeomertys.size() == 0) {
        ImGui::EndChild();
        return;
    }
    static bool affectOriginOnly = false;
    ImGui::Checkbox("Affect origin only?", &affectOriginOnly);

    ImGui::Text("Position");
    ImGui::SliderFloat("p X", &reinterpret_cast<float*>(&TransformData.deltaTransform.position)[0], -100.0f, 100.0f);
    ImGui::SliderFloat("p Y", &reinterpret_cast<float*>(&TransformData.deltaTransform.position)[1], -100.0f, 100.0f);
    ImGui::SliderFloat("p Z", &reinterpret_cast<float*>(&TransformData.deltaTransform.position)[2], -100.0f, 100.0f);
    ImGui::Text("Scale");
    ImGui::SliderFloat("s X", &reinterpret_cast<float*>(&TransformData.deltaTransform.scale)[0], 0.0f, 10.0f);
    ImGui::SliderFloat("s Y", &reinterpret_cast<float*>(&TransformData.deltaTransform.scale)[1], 0.0f, 10.0f);
    ImGui::SliderFloat("s Z", &reinterpret_cast<float*>(&TransformData.deltaTransform.scale)[2], 0.0f, 10.0f);
    ImGui::Text("Rotation");
    ImGui::SliderFloat("r X", &TransformData.DeltaRotationEuler.x, -180.0f, 180.0f);
    ImGui::SliderFloat("r Y", &TransformData.DeltaRotationEuler.y, -180.0f, 180.0f);
    ImGui::SliderFloat("r Z", &TransformData.DeltaRotationEuler.z, -180.0f, 180.0f);
    ImGui::Text("Color");
    ImGui::ColorEdit3("RGB", &TransformData.color.x);
    ImGui::Text("--------Transform--------");

    TransformationMethod& method = this->TransformData.transformationMethod;
    ImGui::RadioButton("None", reinterpret_cast<int*>(&method), static_cast<int>(TransformationMethod::NONE));
    ImGui::RadioButton("Translate", reinterpret_cast<int*>(&method), static_cast<int>(TransformationMethod::TRANSLATE));
    ImGui::RadioButton("Scale", reinterpret_cast<int*>(&method), static_cast<int>(TransformationMethod::SCALE));
    ImGui::RadioButton("Rotation", reinterpret_cast<int*>(&method), static_cast<int>(TransformationMethod::ROTATION));

    for (auto& val : SelectedGeomertys) {
        if (auto wline = dynamic_cast<WidthLine*>(val.first.get())) {
            ImGui::SliderFloat("Width", &wline->width, 0.00f, 10.0f);
        }
    }
    ImGui::EndChild();
}

void CollisionGeoManager::RenewOriginPointPos()
{
    XMVECTOR avgPos = {0.0f,0.0f,0.0f,0.0f};
    for (auto& geo : SelectedGeomertys) {
        avgPos = XMVectorAdd(avgPos, geo.second.position);
    }
    avgPos = XMVectorScale(avgPos, 1.0f / (float)SelectedGeomertys.size());
    XMVECTOR rotation = XMQuaternionIdentity();
    if (SelectedGeomertys.size() == 1) {
        rotation = SelectedGeomertys.front().second.rotation;
    }
    this->TransformData.originTransform = FTransform(avgPos, { 1.0f,1.0f,1.0f,0.0f }, rotation);
}

void CollisionGeoManager::RenewTransformDelta()
{
    TransformData.deltaTransform = FTransform();
    TransformData.DeltaRotationEuler = { 0.0f,0.0f,0.0f };
    TransformData.color = { -1.0f,0.0f,0.0f };
}

void CollisionGeoManager::ApplyTransform(bool renewOrigin)
{
    for (auto& geo : SelectedGeomertys) {
        geo.second = geo.first->GetTransform();
    }
    if (renewOrigin) {
        this->RenewOriginPointPos();
    }
    else
    {
        TransformData.originTransform = TransformData.originTransform + TransformData.deltaTransform;
    }
    TransformData.deltaTransform = FTransform();
}

LineRay::LineRay(screenPos sp, Window& wnd,Camera&cam)
{
    auto clickViewPos = ConvertScreenToView(sp, wnd, 1.0f);
    auto clickWorldPos = ConvertViewToWorld(clickViewPos, wnd);
    XMFLOAT3 rayBeginPos;

    if (IsPerspectiveMatrix(wnd.Gfx().GetProjection())) {
        auto cameraMatrix = wnd.Gfx().GetCamera();
        XMStoreFloat3(&rayBeginPos, cam.GetTransform().position);
    }
    else {
        rayBeginPos = ConvertViewToWorld(ConvertScreenToView(sp, wnd), wnd);
    }

    rayOrigin = rayBeginPos;
    rayDirection = XMFLOAT3(clickWorldPos.x - rayBeginPos.x, clickWorldPos.y - rayBeginPos.y, clickWorldPos.z - rayBeginPos.z);

    XMVECTOR rayDirVec = XMLoadFloat3(&rayDirection);
    rayDirVec = XMVector3Normalize(rayDirVec);
    XMStoreFloat3(&rayDirection, rayDirVec);
}
LineRay::LineRay(DirectX::XMVECTOR origin, DirectX::XMVECTOR direction)
{
    DirectX::XMStoreFloat3(&rayOrigin, origin);
    DirectX::XMStoreFloat3(&rayDirection, direction);
}

XMFLOAT3 ConvertScreenToView(screenPos scPos, Window& wnd, float depthOffset)
{
    auto [width, height] = wnd.GetWndSize();

    float x = (2.0f * scPos.x / width) - 1.0f;
    float y = 1.0f - (2.0f * scPos.y / height);
    XMMATRIX projectionMatrix = wnd.Gfx().GetProjection();
    XMMATRIX invProj = XMMatrixInverse(nullptr, projectionMatrix);

    XMVECTOR screenPosVec = XMVectorSet(x, y, depthOffset, 1.0f);
    XMVECTOR viewPosVec = XMVector3TransformCoord(screenPosVec, invProj);
    return XMFLOAT3(viewPosVec.m128_f32[0], viewPosVec.m128_f32[1], viewPosVec.m128_f32[2]);
}


XMFLOAT3 ConvertViewToWorld(XMFLOAT3 viewPos, Window& wnd)
{
    XMMATRIX viewMatrix = wnd.Gfx().GetCamera();
    XMMATRIX invView = XMMatrixInverse(nullptr, viewMatrix);

    XMVECTOR viewPosVec = XMLoadFloat3(&viewPos);
    XMVECTOR worldPosVec = XMVector3TransformCoord(viewPosVec, invView);

    XMFLOAT3 worldPos;
    XMStoreFloat3(&worldPos, worldPosVec);
    return worldPos;
}

screenPos& screenPos::operator+=(const screenPos& other)
{
    this->x += other.x;
    this->y += other.y;
    return *this;
}

screenPos::screenPos(std::pair<int, int> pos)
{
    x = pos.first;
    y = pos.second;
}

bool screenPos::operator!=(const screenPos& other) const
{
    return x != other.x || y != other.y;
}

screenPos screenPos::operator+(const screenPos& other) const
{
    return screenPos(
    this->x+other.x,
        this->y+other.y
    );
}

screenPos screenPos::operator-(const screenPos& other) const
{
    return screenPos(x - other.x, y - other.y);
}

screenPos& screenPos::operator-=(const screenPos& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}
CollisionGeoManager::TranslationState::TranslationState(Window& window, CollisionGeoManager& manager)
    : InputState(window), collisionManager(manager) {}

void CollisionGeoManager::TranslationState::Enter()
{
    wnd.EnableCursor();
    wnd.mouse.DisableRaw();
    wnd.Kbd.FlushChar();
    wnd.Kbd.FlushKey();
}

void CollisionGeoManager::TranslationState::Update(float deltaTime)
{
    //Handle keyboard input
    while (auto c = wnd.Kbd.ReadChar()) {
        if (collisionManager.GetSelectedGeoNum()) {
            if(shortcutKeyTransformationMethod.find(c.value())!=shortcutKeyTransformationMethod.end())
            this->collisionManager.ChangeTransformationMethod(shortcutKeyTransformationMethod[c.value()]);
        }
    }
    while (auto c = wnd.Kbd.ReadKey())
    {
        if (shortcutKeySwitchPerspective.find(c.value().GetCode()) != shortcutKeySwitchPerspective.end()) {
            collisionManager.cam->SetTransform(shortcutKeySwitchPerspective[c.value().GetCode()]);
        }
        switch (c.value().GetCode()) {
        case VK_RETURN:
            if (!this->selectedComponent) {
                collisionManager.ApplyTransform();
            }
            break;
        }
    }
    //Handling of mouse input
    while (const auto delta = wnd.mouse.Read()) {
        switch (delta->GetType())
        {
        case Mouse::Event::Type::LPress:
        {
            int res = collisionManager.SelectGeometry(delta->GetPos(), wnd);
            if (res == 1) {
                selectedComponent = false;
            }
            else if (res == 2)
            {
                selectedComponent = true;
                wnd.mouse.EnableRaw();
                wnd.mouse.FlushRawDelta();
                if (collisionManager.TransformData.autoApplyTransform)
                {
                    collisionManager.ApplyTransform(false);
                }
            }
        }
        break;

        case Mouse::Event::Type::LRelease:
            if (selectedComponent) {
                selectedComponent = false;
                wnd.mouse.DisableRaw();
                collisionManager.EndComponentTransform();
                if (collisionManager.TransformData.autoApplyTransform) {
                    collisionManager.ApplyTransform(false);
                }
            }
            break;

        case Mouse::Event::Type::RPress:
            if (!selectedComponent) {
                this->Machine->SetState("CameraMove");
            }
            break;
        default:
            break;
        }
    }
    if (selectedComponent && wnd.mouse.LeftIsPressed())
    {
        std::pair<int, int> rawData = { 0,0 };
        while (auto rawDelta = wnd.mouse.ReadRawDelta())
        {
            rawData.first += rawDelta.value().x;
            rawData.second += rawDelta.value().y;
        }
        collisionManager.TransformGeometryByComponent(wnd, { rawData.first,rawData.second });
        wnd.UpdateMousePosition(rawData.first, rawData.second);
    }
    if (!selectedComponent) {
        collisionManager.TransformGeometryByImGui(wnd);
    }
    collisionManager.Transform();
}

void CollisionGeoManager::TranslationState::Exit()
{

}

