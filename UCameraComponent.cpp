#include "UCameraComponent.h"

void UCameraComponent::SetFieldOfView(float FOV)
{
}
// ���캯��
UCameraComponent::UCameraComponent()
    : FieldOfView(90.0f) // Ĭ����Ұ
{
}

UCameraComponent::UCameraComponent(Camera* cam)
    : FieldOfView(90.0f),
    cam(cam)
{
}

// ��������
UCameraComponent::~UCameraComponent() {
}

// ������Ұ
void UCameraComponent::SetFieldOfView(float FOV) {
    FieldOfView = FOV;
}

// ��ȡ��Ұ
float UCameraComponent::GetFieldOfView() const {
    return FieldOfView;
}

// �������λ��
void UCameraComponent::Update(const FTransform& Transform) {
    CurrentTransform = Transform;
    cam->SetTransform(Transform);
}

// ��ȡ��ǰ�任
FTransform UCameraComponent::GetCurrentTransform() const {
    return CurrentTransform;
}

void UCameraComponent::SetCamera(Camera* cam)
{
    this->cam = cam;
}
