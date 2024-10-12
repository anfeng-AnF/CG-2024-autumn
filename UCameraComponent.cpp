#include "UCameraComponent.h"
#include "AActor.h"
#include "imguiManager.h"
#include <DirectXMathVector.inl>
const std::string UCameraComponent::name = "UCameraComponent";

// 构造函数
UCameraComponent::UCameraComponent()
    : FieldOfView(90.0f) // 默认视野
{
}

UCameraComponent::UCameraComponent(std::shared_ptr<Camera> cam)
    : FieldOfView(90.0f),
    cam(std::move(cam))
{
}

// 析构函数
UCameraComponent::~UCameraComponent() {
}

// 设置视野
void UCameraComponent::SetFieldOfView(float FOV) {
    FieldOfView = FOV;
}

// 获取视野
float UCameraComponent::GetFieldOfView() const {
    return FieldOfView;
}

// 更新相机位置
void UCameraComponent::Update(float DeltaTime) {
    auto transforms = Owner->GetComponentFinalTransform();
    this->CurrentTransform = FTransform(transforms[UCameraComponent::name]);

    this->cam->SetTransform(CurrentTransform);

    ImGui::Begin("Test");
    this->cam->SpawnControlWindow();
    ImGui::End();
}

void UCameraComponent::SetTransform(const FTransform& Transform)
{
    CurrentTransform = Transform;
    cam->SetTransform(Transform);
}

// 获取当前变换
FTransform UCameraComponent::GetCurrentTransform() const {
    return cam->GetMatrix();
}

void UCameraComponent::SetCamera(std::shared_ptr<Camera> cam)
{
    this->cam = cam;
}

XMMATRIX UCameraComponent::GetMatrix()
{
    return cam->GetMatrix();
}
