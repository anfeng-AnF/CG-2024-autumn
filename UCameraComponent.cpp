#include "UCameraComponent.h"
#include "AActor.h"
#include "imguiManager.h"
#include <DirectXMathVector.inl>
const std::string UCameraComponent::name = "UCameraComponent";

// ���캯��
UCameraComponent::UCameraComponent()
    : FieldOfView(90.0f) // Ĭ����Ұ
{
}

UCameraComponent::UCameraComponent(std::shared_ptr<Camera> cam)
    : FieldOfView(90.0f),
    cam(std::move(cam))
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

// ��ȡ��ǰ�任
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
