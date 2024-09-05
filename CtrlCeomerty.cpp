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
    std::pair<std::shared_ptr<CollisionGeomerty>, std::vector<CollisionGeomerty::CollisionRes>>* NearestGeo = nullptr;
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
        if (auto a = static_cast<CollisionGeomerty*>(obj.first.get())) {
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
    Geomertys[std::make_shared<CollisionGeomerty>(gfx, _vertexBuffer, _indices, _pos)] = false;
}

 void CtrlGeomerty::AddGeomerty(std::shared_ptr<CollisionGeomerty> Geo)
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
    ImGui::SliderFloat("s X", &reinterpret_cast<float*>(&deltaTransform.scale)[0], -10.0f, 100.0f);
    ImGui::SliderFloat("s Y", &reinterpret_cast<float*>(&deltaTransform.scale)[1], -10.0f, 100.0f);
    ImGui::SliderFloat("s Z", &reinterpret_cast<float*>(&deltaTransform.scale)[2], -10.0f, 100.0f);
    ImGui::Text("Rotation");
    ImGui::SliderFloat("r X", &DeltaRotationEuler.x, -180.0f, 180.0f);
    ImGui::SliderFloat("r Y", &DeltaRotationEuler.y, -180.0f, 180.0f);
    ImGui::SliderFloat("r Z", &DeltaRotationEuler.z, -180.0f, 180.0f);
    ImGui::Text("Color");
    ImGui::ColorEdit3("RGB", &color.x);
    ImGui::Text("--------Transform--------");
    static int choose = 0;
    ImGui::RadioButton("None", &choose, 0);
    ImGui::RadioButton("Translate   ", &choose, 1);
    ImGui::RadioButton("Scale       ", &choose, 2);
    ImGui::RadioButton("Rotation    ", &choose, 3);
    FTransform a= ctrlComponent.GetDeltaTransform();
    XMFLOAT3 forward, right, up;
    XMStoreFloat3(&forward, a.GetForwardVector());
    XMStoreFloat3(&right,   a.GetRightVector());
    XMStoreFloat3(&up,      a.GetUpVector());
    ImGui::Text("Forward Vector: %.2f, %.2f, %.2f", forward.x, forward.y, forward.z);
    ImGui::Text("Right Vector: %.2f, %.2f, %.2f", right.x, right.y, right.z);
    ImGui::Text("Up Vector: %.2f, %.2f, %.2f", up.x, up.y, up.z);
    ImGui::EndChild();
    switch (choose)
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
     pTranslation    = std::make_unique<TransformCtrlComponent>(gfx, posFilePath);
     pScale          = std::make_unique<TransformCtrlComponent>(gfx, scaleFilePath);
     pRotation       = std::make_unique<TransformCtrlComponent>(gfx, rotateFilePath);
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

     XMMATRIX ret=XMMatrixIdentity();
     switch (currentStatue)
     {
     case CtrlComponents::ON_TRANSLATION:
         ret=Translation(deltaPosWorld);
         break;
     case CtrlComponents::ON_SCALE:
         ret=Scale(deltaPosWorld);
         break;
     case CtrlComponents::ON_ROTATION:
         ret=Rotation(deltaPosWorld);
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
 }

 void CtrlComponents::EndTransform()
 {
     isInitialized = false;
     transformAxis = TransformAxis::NONE_AXIS;
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

     Translation = XMMatrixTranslationFromVector(XMVectorScale(projectedDelta,10.0f));
     return Translation;
 }


 XMMATRIX CtrlComponents::Scale(XMVECTOR delta)
 {
     XMMATRIX Translation;
     FTransform componentTransform = pTranslation->GetTransform();
     XMVECTOR forwardVector = componentTransform.GetForwardVector();//target Z
     XMVECTOR rightVector = componentTransform.GetRightVector();    //target X
     XMVECTOR upVector = componentTransform.GetUpVector();          //target Y
     XMFLOAT3 scaleFactor;
     switch (transformAxis)
     {
     case CtrlComponents::NONE_AXIS:
         break;
     case CtrlComponents::X:
         break;
     case CtrlComponents::Y:
         break;
     case CtrlComponents::Z:
         break;
     case CtrlComponents::XY:
         break;
     case CtrlComponents::XZ:
         break;
     case CtrlComponents::YZ:
         break;
     case CtrlComponents::XYZ:
         break;
     }

     return XMMATRIX();
 }

 XMMATRIX CtrlComponents::Rotation(XMVECTOR delta)
 {
     return XMMATRIX();
 }

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
     std::pair<CollisionGeomerty::CollisionRes, cMesh*> hitRes;
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

         return true;
     }
     else
     {
         return false;
     }

 }
