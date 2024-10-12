#include "ACharacter.h"
#include "USpringArmComponent.h"
#include "UCameraComponent.h"
#include "UCapsuleComponent.h"
#include "USkeletalMeshComponent.h"
#include <DirectXMathVector.inl>
ACharacter::ACharacter()
    :
    CharacterMovementComponent(std::make_shared<UCharacterMovementComponent>(this)),
    APawn()
{
    //Ĭ����ӵ��ɱ��齨����������
    AddComponent("", std::make_shared<UCapsuleComponent>(), UCapsuleComponent::name);                       //root
    AddComponent(AActor::RootComponentName, std::make_shared<USpringArmComponent>(), USpringArmComponent::name);
    AddComponent(USpringArmComponent::name, std::make_shared<UCameraComponent>(std::make_shared<Camera>()), UCameraComponent::name);
}

ACharacter::~ACharacter()
{
}
#include"imguiManager.h"
// ÿ֡����
void ACharacter::Tick(float DeltaTime) {
    APawn::Tick(DeltaTime); // ���û���� Tick ����
    CharacterMovementComponent->Update(DeltaTime);

    ImGui::Begin("Gravity dir");
    ImGui::SliderFloat3("", CharacterMovementComponent->GravityDirection.m128_f32, -2.f, 2.f);
    CharacterMovementComponent->GravityDirection = XMVector3Normalize(CharacterMovementComponent->GravityDirection);
    XMFLOAT3 velocityFloat3;
    DirectX::XMStoreFloat3(&velocityFloat3, CharacterMovementComponent->Velocity);

    // ��ӡ Velocity �ķ���
    ImGui::Text("Velocity: X: %.3f, Y: %.3f, Z: %.3f", velocityFloat3.x, velocityFloat3.y, velocityFloat3.z);

    // ���㲢��ӡ Velocity �ĳ���
    float velocityLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(CharacterMovementComponent->Velocity));
    ImGui::Text("Velocity Length: %.3f", velocityLength);
    ImGui::End();

    if (Components.find(USkeletalMeshComponent::name) != Components.end()) {
        if (IsVectorNearZero(CharacterMovementComponent->Velocity))
        {
            return;
        }
        //�����ɫ�ĳ���
        auto VelocityForwardDirection = XMVector3Normalize(
            XMVector3Cross(
                CharacterMovementComponent->GravityDirection,
                XMVector3Cross(
                    CharacterMovementComponent->Velocity,
                    CharacterMovementComponent->GravityDirection
                )
            ));

        //ģ�͵�ǰ����
        auto MeshForward = Components[USkeletalMeshComponent::name]->ActorComponent->RelationTransform.GetForwardVector();

        if (float deltaAngel = CalculateAngleBetweenVectors(MeshForward, VelocityForwardDirection))
        {
            auto quat = FindBetweenNormals(MeshForward, VelocityForwardDirection);
            auto deltaQuat = XMQuaternionSlerp(XMQuaternionIdentity(), quat, min(1.0f, TurnSpeed / deltaAngel));
            Turn(deltaQuat);
        }
    }
}

// ���ý�ɫ�ƶ�������
void ACharacter::Move(const DirectX::XMFLOAT3& Direction) {
    // ������Ը��ݷ�����ٶȸ����ƶ�����
    MovementDirection = Direction; // �����ƶ�����
}

// ���ý�ɫת�������
void ACharacter::Turn(XMVECTOR deltaQuat) {
    if (IsQuaternionNaN(deltaQuat)) {
        int a = 0;
    }
    auto& rotation = Components[USkeletalMeshComponent::name]->ActorComponent->RelationTransform.rotation;
    rotation =XMQuaternionNormalize(DirectX::XMQuaternionMultiply(rotation, deltaQuat)); // ���½�ɫ��ת
}

void ACharacter::AddYawInput(int x)
{
    XMVECTOR UpVector = XMVector3TransformNormal(
        FTransform::UpVector,
        XMMatrixRotationQuaternion(ControllerTransform.rotation));
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(FTransform::UpVector, (float)x/360);
    ControllerTransform.rotation = DirectX::XMQuaternionMultiply(ControllerTransform.rotation, deltaRotation);
    Components[USpringArmComponent::name]->ActorComponent->RelationTransform.rotation = ControllerTransform.rotation;
}

void ACharacter::AddPitchInput(int x)
{
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(ControllerTransform.GetRightVector(), (float)x / 360);
    ControllerTransform.rotation = DirectX::XMQuaternionMultiply(ControllerTransform.rotation, deltaRotation);
    Components[USpringArmComponent::name]->ActorComponent->RelationTransform.rotation = ControllerTransform.rotation;
}

void ACharacter::AddRollInput(int x)
{
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(Transform.GetForwardVector(), (float)x);
    ControllerTransform.rotation = DirectX::XMQuaternionMultiply(ControllerTransform.rotation, deltaRotation);
}

void ACharacter::MoveInput(int x, int y)
{
    //x+ ��ǰ y+ ����
    XMVECTOR input = { x,y,0.0f,0.0f };
    CharacterMovementComponent->SetMovementInput(input);
}

void ACharacter::Jump()
{
    CharacterMovementComponent->Jump();
}

void ACharacter::UpdateJump(float DeltaTime)
{
}

// ���½�ɫ״̬
void ACharacter::UpdateState(ECharacterState state) {
    CurrentState = state;
}

XMMATRIX ACharacter::GetCameraMatrix()
{
    if (Components.find(UCameraComponent::name) != Components.end()) {
        if (auto camComp = dynamic_cast<UCameraComponent*>(Components[UCameraComponent::name]->ActorComponent.get())) {
            return camComp->GetMatrix();
        }
    }
    return XMMatrixIdentity();
}

