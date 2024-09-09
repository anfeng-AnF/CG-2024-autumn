#include "CtrlCeomerty.h"

 CtrlGeomerty::CtrlGeomerty(Camera* cam, Graphics& gfx)
    :
    CurrentCamera(cam),
    gfx(gfx),
    ctrlComponent(gfx,*cam)
{
}

 void CtrlGeomerty::ChangeCamera(Camera* newCam)
{
    CurrentCamera = newCam;
}

 CtrlGeomerty::pGeoPair CtrlGeomerty::TraceByLineNearestGeo()
{
    std::pair<std::shared_ptr<CollisionGeometry>, std::vector<CollisionGeometry::CollisionRes>>* NearestGeo = nullptr;
    float min = D3D11_FLOAT32_MAX;
    for (auto& hit : hitRes) {
        for (auto& hitVal : hit.second) {
            if (min > hitVal.hitDistance) {
                min = hitVal.hitDistance;
                NearestGeo = &hit;
            }
        }
    }
    return NearestGeo;
}

 void CtrlGeomerty::Draw()
{
    for (const auto& obj : Geomertys) {
        if (auto a = static_cast<CollisionGeometry*>(obj.first.get())) {
            a->Bind(gfx);
        }
        obj.first->Draw(gfx);
    }
    for (const auto& obj : DebugGraphs) {
        obj->Draw(gfx);
    }
    ctrlComponent.Draw(gfx);
}

 void CtrlGeomerty::TraceByLine(int click_x, int click_y, const int windowWidth, const int windowHeight, bool isPerspective)
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
    //draw debug line
    XMFLOAT3 rayFarPoint = { 10000 * rayDirection.x,10000 * rayDirection.y ,10000 * rayDirection.z };
    DebugGraphs.push_back(std::make_unique<DebugLine>(gfx, rayOrigin, rayFarPoint, XMFLOAT3{ 1.0f,0.0f,0.0f }));
#endif //_DEBUG

    if (ctrlComponent.TraceByLineSelectTransformAxis(rayOrigin, rayDirection)) {
        //enter to transform model
        ctrlComponent.BeginTransform({ click_x,click_y }, windowWidth, windowHeight);
        this->isUseCtrlComponent = true;
        ctrlComponent.SetTransform(currentTransformReference);
        ctrlComponent.SetBeginTransform(currentTransformReference);
        return;
    }

    this->isUseCtrlComponent = false;
    this->hitRes.clear();
    for (auto& obj : Geomertys) {
        auto hitResult = obj.first->TraceByLine(rayOrigin, rayDirection, obj.first->GetTransformXM());

        if (hitResult.size() == 0)continue;
        this->hitRes.push_back({ obj.first,hitResult });
        for (auto& val : hitResult) {
#ifdef _DEBUG
            this->DebugGraphs.push_back(std::make_unique<DebugSphere>(gfx, XMFLOAT3{ 0.0f,0.0f,1.0f }, val.pos, 0.05f));
#endif // _DEBUG
        }
    }
}

 void CtrlGeomerty::AddGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
    Geomertys[std::make_shared<CollisionGeometry>(gfx, _vertexBuffer, _indices, _pos)] = false;
}

 void CtrlGeomerty::AddGeomerty(std::shared_ptr<CollisionGeometry> Geo)
{
    Geomertys[std::move(Geo)] = false;
}

 bool CtrlGeomerty::SelectGeomerty(int click_x, int click_y, const int windowWidth, const int windowHeight, bool isPerspective)
 {
     //everytime we select,whither selected or not ,we renew old geo transform,and reset deltaTransform and other Attribute
     deltaTransform = FTransform{};
     DeltaRotationEuler = { 0.0f,0.0f,0.0f };
     color = { -1.0f,-1.0f,-1.0f };
     for (auto& val : mapSelectedGeomertys) {
         val.second = val.first->GetTransform();
     }

     TraceByLine(click_x, click_y, windowWidth, windowHeight, isPerspective);
     if (this->isUseCtrlComponent)return true;
     pGeoPair pNearestGeo = TraceByLineNearestGeo();

     //if none geo selected,set no selected
     if (!pNearestGeo) {
         mapSelectedGeomertys.clear();
         for (auto& geo : Geomertys) {
             if (geo.second) {
                 geo.first->SetSelect(false);
                 geo.second = false;
                 selectedGeoNum--;
             }
         }
         ctrlComponent.SetStatue(CtrlComponents::NONE);
         return false;
     }

     if (Geomertys[pNearestGeo->first]) {
         Geomertys[pNearestGeo->first] = false;
         pNearestGeo->first->SetSelect(false);
         selectedGeoNum--;
         mapSelectedGeomertys.erase(pNearestGeo->first);
     }
     else
     {
         Geomertys[pNearestGeo->first] = true;
         pNearestGeo->first->SetSelect(true);
         selectedGeoNum++;
         auto a = mapSelectedGeomertys.find(pNearestGeo->first);
         mapSelectedGeomertys[pNearestGeo->first] = pNearestGeo->first->GetTransform();
     }

     //if selected anything ,recaculate transfrom origin point transform

     XMVECTOR pos = { 0.0f,0.0f,0.0f,0.0f };
     for (auto& geo : this->mapSelectedGeomertys) {
         //apply transform
         pos = XMVectorAdd(pos, geo.second.position);
         geo.second = geo.first->GetTransform();
     }
     pos = XMVectorScale(pos, 1.0f / this->mapSelectedGeomertys.size());
     currentTransformReference.position = pos;
     //if selected only one geo,use it's rotation 
     if (this->mapSelectedGeomertys.size() == 1) {
         currentTransformReference.rotation = mapSelectedGeomertys.begin()->second.rotation;
     }
     ctrlComponent.SetTransform(currentTransformReference);
     ctrlComponent.SetBeginTransform(currentTransformReference);

     return true;
 }

 void CtrlGeomerty::TransformGeomerty(Window& wnd)
{
    std::ostringstream oss;
    for (auto& val : mapSelectedGeomertys) {
        oss << typeid(*val.first.get()).name() << std::endl;
    }
    ImGui::BeginChild("transform", { 0,0 });
    if (selectedGeoNum == 0) {
        ImGui::EndChild();
        return;
    }
    static bool affectOriginOnly=false;
    ImGui::Checkbox("Affect origin only?", &affectOriginOnly);

    ImGui::Text(oss.str().c_str());
    ImGui::Text("Position");
    ImGui::SliderFloat("p X", &reinterpret_cast<float*>(&deltaTransform.position)[0], -100.0f, 100.0f);
    ImGui::SliderFloat("p Y", &reinterpret_cast<float*>(&deltaTransform.position)[1], -100.0f, 100.0f);
    ImGui::SliderFloat("p Z", &reinterpret_cast<float*>(&deltaTransform.position)[2], -100.0f, 100.0f);
    ImGui::Text("Scale");
    ImGui::SliderFloat("s X", &reinterpret_cast<float*>(&deltaTransform.scale)[0], 0.0f, 10.0f);
    ImGui::SliderFloat("s Y", &reinterpret_cast<float*>(&deltaTransform.scale)[1], 0.0f, 10.0f);
    ImGui::SliderFloat("s Z", &reinterpret_cast<float*>(&deltaTransform.scale)[2], 0.0f, 10.0f);
    ImGui::Text("Rotation");
    ImGui::SliderFloat("r X", &DeltaRotationEuler.x, -180.0f, 180.0f);
    ImGui::SliderFloat("r Y", &DeltaRotationEuler.y, -180.0f, 180.0f);
    ImGui::SliderFloat("r Z", &DeltaRotationEuler.z, -180.0f, 180.0f);
    ImGui::Text("Color");
    ImGui::ColorEdit3("RGB", &color.x);
    ImGui::Text("--------Transform--------");
    transformationMethod;
    ImGui::RadioButton("None", &transformationMethod, 0);
    ImGui::RadioButton("Translate   ", &transformationMethod, 1);
    ImGui::RadioButton("Rotation    ", &transformationMethod, 3);
    ImGui::RadioButton("Scale       ", &transformationMethod, 2);
    FTransform a= ctrlComponent.GetDeltaTransform();
    XMFLOAT3 forward, right, up;
    XMStoreFloat3(&forward, a.GetForwardVector());
    XMStoreFloat3(&right,   a.GetRightVector());
    XMStoreFloat3(&up,      a.GetUpVector());
    ImGui::Text("Forward Vector: %.2f, %.2f, %.2f", forward.x, forward.y, forward.z);
    ImGui::Text("Right Vector: %.2f, %.2f, %.2f", right.x, right.y, right.z);
    ImGui::Text("Up Vector: %.2f, %.2f, %.2f", up.x, up.y, up.z);
    for (auto& val : mapSelectedGeomertys) {
        if (auto wline = dynamic_cast<WidthLine*>(val.first.get())) {
            ImGui::SliderFloat("Width", &wline->width, 0.00f, 10.0f);
        }
    }
    ImGui::EndChild();
    switch (transformationMethod)
    {
    case 0:
        ctrlComponent.SetStatue(CtrlComponents::NONE);
        break;
    case 1:
        ctrlComponent.SetStatue(CtrlComponents::ON_TRANSLATION);
        break;
    case 2:
        ctrlComponent.SetStatue(CtrlComponents::ON_SCALE);
        break;
    case 3:
        ctrlComponent.SetStatue(CtrlComponents::ON_ROTATION);
        break;
    }
    currentTransformReference = deltaTransform + ctrlComponent.GetBeginTransform();
    ctrlComponent.SetTransform(currentTransformReference);
    deltaTransform.rotation = XMQuaternionRotationRollPitchYaw(DeltaRotationEuler.x, DeltaRotationEuler.y, DeltaRotationEuler.z);
    for (auto& val : mapSelectedGeomertys) {
        FTransform newTransform = deltaTransform + val.second;
        val.first->SetTransform(newTransform);
        if (color.x != -1.0f) {
            val.first->SetColor(color);
        }
    }
}

 void CtrlGeomerty::ChangeTransformationMethod(const char buffer)
 {
     switch (buffer)
     {
     case 'w':
     case 'W':
         transformationMethod = 1;  // Translation
         break;
     case 'e':
     case 'E':
         transformationMethod = 3;  // Rotation
         break;
     case 'r':
     case 'R':
         transformationMethod = 2;  // Scale
         break;
     case 'q':
     case 'Q':
         transformationMethod = 0;  // Select (None)
         break;
     default:
         break;
     }
 }

 void CtrlGeomerty::EndComponentTransform()
 {
     ctrlComponent.EndTransform();
     this->isUseCtrlComponent = false;
 }

 bool CtrlGeomerty::IsUseCtrlComponent()
 {
     return isUseCtrlComponent;
 }

 void CtrlGeomerty::OnTransformComponent(std::pair<int, int> screenPos, int wndWidth, int wndHeight)
 {
     deltaTransform =FTransform(ctrlComponent.Transform(screenPos, wndWidth, wndHeight));
 }

 CtrlComponents::CtrlComponents(Graphics& gfx,Camera&cam, std::string posFilePath, std::string scaleFilePath, std::string rotateFilePath)
     :
     gfx(gfx),
     cam(cam)
 {
     pTranslation    = std::make_unique<TransformCtrlComponent>(gfx,cam, posFilePath);
     pScale          = std::make_unique<TransformCtrlComponent>(gfx,cam, scaleFilePath);
     pRotation       = std::make_unique<TransformCtrlComponent>(gfx,cam, rotateFilePath,0.015f);
 }

 void CtrlComponents::Draw(Graphics& gfx) noexcept
 {
     auto depth = Bind::DepthStencilState(gfx);
     depth.Bind(gfx);
     pTranslation->SetTransform(transform);
     pScale->SetTransform(transform);
     pRotation->SetTransform(transform);
     switch (currentStatue)
     {
     case CtrlComponents::NONE:
         break;
     case CtrlComponents::ON_TRANSLATION:
         pTranslation->Draw(gfx);
         break;
     case CtrlComponents::ON_SCALE:
         pScale->Draw(gfx);
         break;
     case CtrlComponents::ON_ROTATION:
         pRotation->Draw(gfx);
         break;
     }
     depth.UnBind(gfx);
 }

 void CtrlComponents::SetStatue(CtrlComponents::Statue curStatue)
 {
     this->currentStatue = curStatue;
 }

 XMMATRIX CtrlComponents::Transform(std::pair<int, int> dTransformPosScreen, int wndWidth, int wndHeight)
 {
     assert(isInitialized && "forget call BeginTransform() before?");
     this->deltaTransfeomPosScreen.first += dTransformPosScreen.first;
     this->deltaTransfeomPosScreen.second += dTransformPosScreen.second;
     auto posWorld = this->ScreenToWorld(deltaTransfeomPosScreen, wndWidth, wndHeight);
     auto deltaPosWorld = XMVectorSubtract(posWorld, BeginPosWorld);

     XMMATRIX ret = XMMatrixIdentity();
     switch (currentStatue)
     {
     case CtrlComponents::ON_TRANSLATION:
         ret = Translation(deltaPosWorld);
         break;
     case CtrlComponents::ON_SCALE:
         ret = Scale(deltaPosWorld);
         break;
     case CtrlComponents::ON_ROTATION:
     {
         ret=Rotation(wndWidth, wndHeight);
     }
     break;
     default:
         break;
     }
     //this->transform =FTransform(ret)+beginTransform;
     return ret;
 }

 void CtrlComponents::BeginTransform(std::pair<int, int> bTransformPosScreen, int wndWidth, int wndHeight)
 {

     this->beginTransformPosScreen = bTransformPosScreen;
     this->deltaTransfeomPosScreen = bTransformPosScreen;
     this->BeginPosWorld = this->ScreenToWorld(bTransformPosScreen,wndWidth,wndHeight);
     isInitialized = true;
     switch (currentStatue)
     {
     case CtrlComponents::NONE:
         break;
     case CtrlComponents::ON_TRANSLATION:
         this->BeginTransformScale = pTranslation->GetDynamicScaling(gfx, cam);
         break;
     case CtrlComponents::ON_SCALE:
         this->BeginTransformScale = pScale->GetDynamicScaling(gfx, cam);
         break;
     case CtrlComponents::ON_ROTATION:
     {

         auto line = ScreenToWorldRay(bTransformPosScreen, wndWidth, wndHeight, cam.GetMatrix(), gfx.GetProjection(), true);
         switch (transformAxis)
         {
         case CtrlComponents::X:
             this->BeginRotationPosWorld = this->RayIntersectsPlane(line.first, line.second, transform.position, transform.GetRightVector()).value();
             break;
         case CtrlComponents::Y:
             this->BeginRotationPosWorld = this->RayIntersectsPlane(line.first, line.second, transform.position, transform.GetUpVector()).value();
             break;
         case CtrlComponents::Z:
             this->BeginRotationPosWorld = this->RayIntersectsPlane(line.first, line.second, transform.position, transform.GetForwardVector()).value();
             break;
         default:
             break;
         }
     }
         break;
     default:
         break;
     }
 }

 void CtrlComponents::EndTransform()
 {
     isInitialized = false;
     transformAxis = TransformAxis::NONE_AXIS;
     this->selectedCMesh->SetSelect(false);
 }

 XMMATRIX CtrlComponents::Translation(XMVECTOR delta)
 {
     XMMATRIX Translation=XMMatrixIdentity();
     FTransform componentTransform = pTranslation->GetTransform();

     XMVECTOR projectedDelta = XMVectorZero();
     switch (transformAxis)
     {
     case CtrlComponents::NONE_AXIS:
         break;

     case CtrlComponents::X:
         projectedDelta = XMVectorScale(componentTransform.GetRightVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetRightVector())));
         break;

     case CtrlComponents::Y:
         projectedDelta = XMVectorScale(componentTransform.GetUpVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetUpVector())));
         break;

     case CtrlComponents::Z:
         projectedDelta = XMVectorScale(componentTransform.GetForwardVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetForwardVector())));
         break;

     case CtrlComponents::XY:
         projectedDelta = XMVectorAdd(
             XMVectorScale(componentTransform.GetRightVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetRightVector()))),
             XMVectorScale(componentTransform.GetUpVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetUpVector())))
         );
         break;

     case CtrlComponents::XZ:
         projectedDelta = XMVectorAdd(
             XMVectorScale(componentTransform.GetRightVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetRightVector()))),
             XMVectorScale(componentTransform.GetForwardVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetForwardVector())))
         );
         break;

     case CtrlComponents::YZ:
         projectedDelta = XMVectorAdd(
             XMVectorScale(componentTransform.GetUpVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetUpVector()))),
             XMVectorScale(componentTransform.GetForwardVector(), XMVectorGetX(XMVector3Dot(delta, componentTransform.GetForwardVector())))
         );
         break;

     case CtrlComponents::XYZ:
         projectedDelta = delta;
         break;
     }

     Translation = XMMatrixTranslationFromVector(XMVectorScale(projectedDelta,20.0f*BeginTransformScale));
     return Translation;
 }


#define GetScale(delta,vector) std::abs(XMVectorScale(XMVector3Dot(delta, rightVector),XMVector3Length(rightVector).m128_f32[0]).m128_f32[0])
 XMMATRIX CtrlComponents::Scale(XMVECTOR delta)
 {
     XMMATRIX Translation;
     FTransform componentTransform = pTranslation->GetTransform();
     XMVECTOR forwardVector = componentTransform.GetForwardVector();//target Z
     XMVECTOR rightVector = componentTransform.GetRightVector();    //target X
     XMVECTOR upVector = componentTransform.GetUpVector();          //target Y
     XMFLOAT3 scaleFactor = { 1.0f,1.0f,1.0f};
     delta = XMVectorAdd(delta, forwardVector);
     delta = XMVectorAdd(delta, rightVector);
     delta = XMVectorAdd(delta, upVector);
     switch (transformAxis)
     {
     case CtrlComponents::NONE_AXIS:
         break;
     case CtrlComponents::X:
         scaleFactor.x =GetScale(delta,rightVector);
         break;
     case CtrlComponents::Y:
         scaleFactor.y = GetScale(delta, upVector);
         break;
     case CtrlComponents::Z:
         scaleFactor.z = GetScale(delta,forwardVector);
         break;
     case CtrlComponents::XY:
         scaleFactor.x = GetScale(delta, rightVector);
         scaleFactor.y = GetScale(delta, upVector);
         break;
     case CtrlComponents::XZ:
         scaleFactor.x = GetScale(delta, rightVector);
         scaleFactor.z = GetScale(delta, forwardVector);
         break;
     case CtrlComponents::YZ:
         scaleFactor.y = GetScale(delta, upVector);
         scaleFactor.z = GetScale(delta, forwardVector);
         break;
     case CtrlComponents::XYZ:
         scaleFactor.x = GetScale(delta, rightVector);
         scaleFactor.y = GetScale(delta, upVector);
         scaleFactor.z = GetScale(delta, forwardVector);
         break;
     }
     
     return XMMatrixScaling(scaleFactor.x,scaleFactor.y,scaleFactor.z);
 }
 #undef GetScale(delta,vector) XMVectorScale(XMVector3Dot(delta, rightVector),XMVector3Length(rightVector).m128_f32[0]).m128_f32[0]


#define computeAngel(vec) curPos=this->RayIntersectsPlane(line.first, line.second, transform.position, vec).value();\
 angel = computeRotationAngle(XMVectorSubtract(XMLoadFloat3(&BeginRotationPosWorld), transform.position), XMVectorSubtract(XMLoadFloat3(&curPos), transform.position), vec);

 XMMATRIX CtrlComponents::Rotation(int wndWidth, int wndHeight)
 {
     auto line = this->ScreenToWorldRay(
         { deltaTransfeomPosScreen.first + this->beginTransformPosScreen.first,
         deltaTransfeomPosScreen.second + this->beginTransformPosScreen.second }, 
         wndWidth, wndHeight, cam.GetMatrix(), gfx.GetProjection(), true);

     auto computeRotationAngle = [](const XMVECTOR fromVec, const XMVECTOR toVec,const XMVECTOR planeNormal) -> float
         {
             // 归一化输入向量
             XMVECTOR normFromVec = XMVector3Normalize(fromVec);
             XMVECTOR normToVec = XMVector3Normalize(toVec);

             // 计算旋转轴（叉积）
             XMVECTOR rotationAxis = XMVector3Cross(normFromVec, normToVec);

             // 计算点积
             float dotProduct = XMVectorGetX(XMVector3Dot(normFromVec, normToVec));

             // 夹角的弧度
             float angle = std::acos(std::clamp(dotProduct, -1.0f, 1.0f));

             // 计算旋转方向的标志（正向还是负向）
             float sign = XMVectorGetX(XMVector3Dot(rotationAxis, planeNormal));
             if (sign < 0.0f)
             {
                 angle = -angle; // 如果叉积方向与法向量方向相反，旋转角度为负
             }

             return angle;
         };
     XMFLOAT3 curPos;
     float angel = 0.0f;
     switch (transformAxis)
     {
     case CtrlComponents::X:
         computeAngel(transform.GetRightVector());
         return XMMatrixRotationRollPitchYaw(angel,0.0f,0.0f);
         break;
     case CtrlComponents::Y:
         computeAngel(transform.GetUpVector());
         return XMMatrixRotationRollPitchYaw(0.0f ,angel, 0.0f);
         break;
     case CtrlComponents::Z:
         computeAngel(transform.GetForwardVector());
         return XMMatrixRotationRollPitchYaw(0.0f, 0.0f, angel);
         break;
     default:
         return XMMatrixIdentity();
         break;
     }
 }
#undef computeAngel(vec) curPos=this->RayIntersectsPlane(line.first, line.second, transform.position, vec).value();\
 angel = computeRotationAngle(XMVectorSubtract(XMLoadFloat3(&BeginRotationPosWorld), transform.position), XMVectorSubtract(XMLoadFloat3(&curPos), transform.position), vec);

 XMVECTOR CtrlComponents::ScreenToWorld(std::pair<int, int> deltaPosScreen, int wndWidth, int wndHeight)
 {
     XMVECTOR retVector;
     XMMATRIX viewMatrix = cam.GetMatrix();
     XMMATRIX projectionMatrix = gfx.GetProjection();
     
     float ndcX = (2.0f * deltaPosScreen.first) / wndWidth - 1.0f;
     float ndcY = 1.0f - (2.0f * deltaPosScreen.second) / wndHeight;
     float ndcZ = 1.0f;

     XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
     XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);

     XMVECTOR rayNDC = XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);
     XMVECTOR rayViewSpace = XMVector3TransformCoord(rayNDC, invProjMatrix);

     XMVECTOR rayWorldSpace = XMVector3TransformCoord(rayViewSpace, invViewMatrix);
     XMVECTOR cameraPosition = invViewMatrix.r[3];

     retVector = XMVectorAdd(cameraPosition,XMVector3Normalize(rayWorldSpace));

     return retVector;
 }

 FTransform CtrlComponents::GetDeltaTransform()
 {
     switch (currentStatue)
     {
     case CtrlComponents::NONE:
         return FTransform();
         break;
     case CtrlComponents::ON_TRANSLATION:
         return pTranslation->GetTransform();
         break;
     case CtrlComponents::ON_SCALE:
         return pScale->GetTransform();
         break;
     case CtrlComponents::ON_ROTATION:
         return pRotation->GetTransform();
         break;
     }
 }

 void CtrlComponents::SetTransform(FTransform trans)
 {
     transform = trans;
 }

 FTransform CtrlComponents::GetBeginTransform()
 {
     return beginTransform;
 }

 void CtrlComponents::SetBeginTransform(FTransform trans)
 {
     beginTransform = trans;
 }

 bool CtrlComponents::TraceByLineSelectTransformAxis(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
 {
     std::pair<CollisionGeometry::CollisionRes, cMesh*> hitRes;
     switch (currentStatue)
     {
     case CtrlComponents::NONE:
         break;
     case CtrlComponents::ON_TRANSLATION:
         hitRes=pTranslation->TraceByLineGetNearestMesh(lineBeginPos,lineVector);
         break;
     case CtrlComponents::ON_SCALE:
         hitRes=pScale->TraceByLineGetNearestMesh(lineBeginPos,lineVector);
         break;
     case CtrlComponents::ON_ROTATION:
         hitRes=pRotation->TraceByLineGetNearestMesh(lineBeginPos,lineVector);
         break;
     }

     if (hitRes.second) {
         auto name=hitRes.second->GetName();
         if (name == "X")transformAxis = X;
         if (name == "Y")transformAxis = Y;
         if (name == "Z")transformAxis = Z;
         if (name == "XY")transformAxis = XY;
         if (name == "XZ")transformAxis = XZ;
         if (name == "YZ")transformAxis = YZ;
         if (name == "XYZ")transformAxis = XYZ;
         hitRes.second->SetSelect(true);

         this->selectedCMesh = hitRes.second;
         return true;
     }
     else
     {
         return false;
     }

 }

 std::pair<XMFLOAT3, XMFLOAT3> CtrlComponents::ScreenToWorldRay(const std::pair<int, int>& screenPos, int wndWidth, int wndHeight, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isPerspective)
 {
     XMFLOAT3 rayOrigin, rayDirection;

     // 计算标准化设备坐标 (NDC)
     float ndcX = (2.0f * screenPos.first) / wndWidth - 1.0f;
     float ndcY = 1.0f - (2.0f * screenPos.second) / wndHeight;
     float ndcZ = 1.0f; // 对于透视投影

     // 计算反变换矩阵
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
         // 正交投影的情况
         rayDirection = { 0.0f, 0.0f, 1.0f }; // 正交投影下，射线方向是相机前方的方向
         XMStoreFloat3(&rayOrigin, invViewMatrix.r[3]);
     }

     return { rayOrigin, rayDirection };
 }

 std::optional<XMFLOAT3> CtrlComponents::RayIntersectsPlane(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, const XMVECTOR& planePointVec, const XMVECTOR& planeNormalVec)
 {
     // 将输入的浮点数向量转换为 XMVECTOR
     XMVECTOR rayOriginVec = XMLoadFloat3(&rayOrigin);
     XMVECTOR rayDirectionVec = XMLoadFloat3(&rayDirection);

     // 计算射线方向与面法向量的点积
     float denominator = XMVectorGetX(XMVector3Dot(rayDirectionVec, planeNormalVec));

     // 如果点积接近零，则射线与面平行
     if (std::fabs(denominator) < 1e-6f)
     {
         return std::nullopt; // 没有交点
     }

     // 计算射线起点到面上的点的向量
     XMVECTOR rayToPlaneVec = planePointVec - rayOriginVec;

     // 计算参数 t
     float t = XMVectorGetX(XMVector3Dot(rayToPlaneVec, planeNormalVec)) / denominator;

     // 如果 t 小于 0，则交点在射线的反向方向
     if (t < 0.0f)
     {
         return std::nullopt; // 没有交点
     }

     // 计算交点位置
     XMVECTOR intersectionVec = rayOriginVec + t * rayDirectionVec;
     XMFLOAT3 intersectionPoint;
     XMStoreFloat3(&intersectionPoint, intersectionVec);

     return intersectionPoint; // 返回交点坐标
 }
