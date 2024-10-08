#include "USpringArmComponent.h"

USpringArmComponent::USpringArmComponent()
{
}

USpringArmComponent::~USpringArmComponent()
{
}
// 构造函数
USpringArmComponent::USpringArmComponent()
    : ArmLength(300.0f) // 默认长度
{
}

// 析构函数
USpringArmComponent::~USpringArmComponent() {
}

void USpringArmComponent::SetArmLength(float Length) {
    ArmLength = Length;
}

float USpringArmComponent::GetArmLength() const {
    return ArmLength;
}

void USpringArmComponent::Update(const FTransform& OwnerTransform) {
    CurrentTransform = OwnerTransform;
    float* z;
    XMVectorGetZPtr(z, CurrentTransform.position);
    *z += ArmLength;
}

// 获取当前位置
FTransform USpringArmComponent::GetCurrentTransform() const {
    return CurrentTransform;
}