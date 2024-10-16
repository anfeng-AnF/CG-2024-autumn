#include "USpringArmComponent.h"
#include "AActor.h"

const std::string USpringArmComponent::name = "USpringArmComponent";

// 构造函数
USpringArmComponent::USpringArmComponent()
    : ArmLength(-30.0f) // 默认长度
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

}

// 获取当前位置
FTransform USpringArmComponent::GetCurrentTransform() const {
    return CurrentTransform;
}

DirectX::XMMATRIX USpringArmComponent::GetRelationTransformMatrix()
{
    return
        DirectX::XMMatrixTranslation(0.0f, 0.0f, ArmLength) *
        RelationTransform.GetMatrix();
}

void USpringArmComponent::Update(float DeltaTime)
{
    auto transforms = Owner->GetComponentFinalTransform();
    this->CurrentTransform = FTransform(transforms[USpringArmComponent::name]);
}
