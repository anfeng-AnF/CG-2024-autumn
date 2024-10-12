#include "USpringArmComponent.h"
#include "AActor.h"

const std::string USpringArmComponent::name = "USpringArmComponent";

// ���캯��
USpringArmComponent::USpringArmComponent()
    : ArmLength(-30.0f) // Ĭ�ϳ���
{
    RelationTransform.position = XMVectorSetByIndex(RelationTransform.position, -30.0f, 2);
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

DirectX::XMMATRIX USpringArmComponent::GetRelationTransformMatrix()
{
    return
        RelationTransform.GetTranslateMatrix() *
        RelationTransform.GetScaleMatrix() *
        RelationTransform.GetRotationMatrix();
}

void USpringArmComponent::Update(float DeltaTime)
{
}
