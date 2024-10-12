#include "USpringArmComponent.h"
#include "AActor.h"

const std::string USpringArmComponent::name = "USpringArmComponent";

// ���캯��
USpringArmComponent::USpringArmComponent()
    : ArmLength(-30.0f) // Ĭ�ϳ���
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

}

// ��ȡ��ǰλ��
FTransform USpringArmComponent::GetCurrentTransform() const {
    return CurrentTransform;
}

void USpringArmComponent::Update(float DeltaTime)
{
    auto self = Owner->GetComponentInfo(USpringArmComponent::name);
    self->RelateTransfom.position = XMVectorSetByIndex(self->RelateTransfom.position, ArmLength, 2);
}
