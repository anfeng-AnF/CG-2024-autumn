#include "USpringArmComponent.h"

USpringArmComponent::USpringArmComponent()
{
}

USpringArmComponent::~USpringArmComponent()
{
}
// ���캯��
USpringArmComponent::USpringArmComponent()
    : ArmLength(300.0f) // Ĭ�ϳ���
{
}

// ��������
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

// ��ȡ��ǰλ��
FTransform USpringArmComponent::GetCurrentTransform() const {
    return CurrentTransform;
}