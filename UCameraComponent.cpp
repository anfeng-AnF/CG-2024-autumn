#include "UCameraComponent.h"

void UCameraComponent::SetFieldOfView(float FOV)
{
}
// 构造函数
UCameraComponent::UCameraComponent()
    : FieldOfView(90.0f) // 默认视野
{
}

UCameraComponent::UCameraComponent(Camera* cam)
    : FieldOfView(90.0f),
    cam(cam)
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
void UCameraComponent::Update(const FTransform& Transform) {
    CurrentTransform = Transform;
    cam->SetTransform(Transform);
}

// 获取当前变换
FTransform UCameraComponent::GetCurrentTransform() const {
    return CurrentTransform;
}

void UCameraComponent::SetCamera(Camera* cam)
{
    this->cam = cam;
}
