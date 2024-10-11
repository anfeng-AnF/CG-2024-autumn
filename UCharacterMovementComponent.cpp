#include "UCharacterMovementComponent.h"
#include <DirectXMathVector.inl>

const std::string UCharacterMovementComponent::name = "UCharacterMovementComponent";

// ���캯��
UCharacterMovementComponent::UCharacterMovementComponent()
    : Velocity(XMVectorZero()), // ��ʼ���ٶ�Ϊ������
    MovementInput(XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f)), // ��ʼ���ƶ�����Ϊ��
    JumpHeight(300.0f), // ������Ծ�߶�
    bIsOnGround(true), // Ĭ���ڵ�����
    GravityScale(9.81f) // �����������ٶ�
{}

// ��������
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// ÿ֡����
void UCharacterMovementComponent::Update(float DeltaTime) {
    // ���������ٶȣ�����ײ
    UpdateVelocity(DeltaTime);

    // ����λ��
    UpdatePosition(DeltaTime);
}

// �����ƶ�����
void UCharacterMovementComponent::SetMovementInput(const XMFLOAT3& Direction) {
    MovementInput = XMVectorSet(Direction.x, Direction.y, Direction.z, 0.0f);
}

// ��Ծ
void UCharacterMovementComponent::Jump() {
    Velocity += (-GravityDirection) * JumpHeight;
    Velocity = XMVector3Normalize(Velocity);
}

// ʩ������
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    Velocity = DirectX::XMVectorAdd(Velocity, DirectX::XMVectorScale(GravityDirection, GravityScale));
}

// ��ȡ��ǰ�ٶ�
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}


bool UCharacterMovementComponent::IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle) {
    float DotProduct = DirectX::XMVectorGetY(Normal);
    float SlopeAngle = acosf(DotProduct) * (180.0f / DirectX::XM_PI); // �����¶ȽǶ�
    return DotProduct > FloorDetectionThreshold && SlopeAngle <= MaxSlopeAngle;
}
// ����λ��
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {
    auto transform = Owner->GetTransform();
    transform.position += Velocity * Speed * DeltaTime + transform.position;
    Owner->SetTransform(transform);
}

void UCharacterMovementComponent::UpdateVelocity(float DeltaTime)
{
    // ȷ����ɫӵ���ߴ���
    if (!Owner) return;

    // ��ѯ��ײ��Ϣ
    auto World = Owner->GetWorld();
    if (!World) return;

    std::vector<FCollisionResult> CollisionResults = World->QuaryCollision(Owner);

    // ���û�з�����ײ�����������״̬
    if (CollisionResults.empty()) {
        // Ӧ�����������ٶ�
        ApplyGravity(DeltaTime);
        return;
    }

    DirectX::XMVECTOR GroundNormal = DirectX::XMVectorZero();
    DirectX::XMVECTOR WallNormal = DirectX::XMVectorZero();
    bool bOnGround = false;

    // ����������ײ��������ֵ����ǽ��
    for (const auto& CollisionResult : CollisionResults) {
        DirectX::XMVECTOR Normal = XMVectorSet(CollisionResult.HitResult.Normal.x, CollisionResult.HitResult.Normal.y, CollisionResult.HitResult.Normal.z, 0.0f);

        if (IsWalkableSurface(Normal, MaxSlopeAngle)) {
            // ����ǿ����ߵĵ��棬�ۼӵ��淨�߲����Ϊ�ڵ�����
            GroundNormal = DirectX::XMVectorAdd(GroundNormal, Normal);
            bOnGround = true;
        }
        else {
            // �����ǽ�棬�ۼ�ǽ�淨��
            WallNormal = DirectX::XMVectorAdd(WallNormal, Normal);
        }
    }
    // ����ڵ����ϣ�ֹͣ���䲢�Ƴ��ٶ�������������ķ���
    if (bOnGround) {
        // ��һ�����淨��
        GroundNormal = DirectX::XMVector3Normalize(GroundNormal);

        // ���ٶ���������������ķ�������Ϊ�㣨ֹͣ���䣩
        DirectX::XMVECTOR GravityVelocity = DirectX::XMVector3Dot(Velocity, GravityDirection) * GravityDirection;
        Velocity -= GravityVelocity;
    }
    else {
        // ������ڵ����ϣ��������ٶ���ǽ�淨���ϵķ����Ƴ�
        WallNormal = DirectX::XMVector3Normalize(WallNormal);
        Velocity -= DirectX::XMVector3Dot(Velocity, WallNormal) * WallNormal;

        // �������Ӱ��
        Velocity += GravityDirection * GravityScale * DeltaTime;
    }
    Velocity = XMVector3Normalize(Velocity);
}

