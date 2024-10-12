#include "UCharacterMovementComponent.h"
#include <DirectXMathVector.inl>
#include "World.h"
#include "ACharacter.h"
#include "USpringArmComponent.h"
const std::string UCharacterMovementComponent::name = "UCharacterMovementComponent";

UCharacterMovementComponent::UCharacterMovementComponent(AActor* Owner)
    : Velocity(XMVectorZero()), // ��ʼ���ٶ�Ϊ������
    JumpInitialSpeed(300.0f), // ������Ծ�߶�
    bIsOnGround(true), // Ĭ���ڵ�����
    GravityAcceleration(9.81f), // �����������ٶ�
    GravityDirection({ 0.f,-1.f,0.f,0.f }),
    Acceleration({ 0.f,0.f,0.f,0.f }),
    InputAcceration({ 0.f,0.f,0.f,0.f })
{
    this->Owner = Owner;
}

// ��������
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// ÿ֡����
void UCharacterMovementComponent::Update(float DeltaTime) {
    //������ٶ�
    UpdateAcceleration(DeltaTime);

    // ���������ٶȣ�����ײ
    UpdateVelocity(DeltaTime);

    // ����λ��
    UpdatePosition(DeltaTime);
}

// �����ƶ�����
void UCharacterMovementComponent::SetMovementInput(const XMVECTOR& Direction) {
    if (XMVectorGetX(Direction) == 0 && XMVectorGetY(Direction) == 0) {
        bIsInputAcceration = false;
        InputAcceration={ 0.f,0.f,0.f,0.f };
        return;
    }
    if (auto Character = static_cast<ACharacter*>(Owner)) {
        auto LookAtRightVector = FTransform(Character->GetComponentFinalTransform()[USpringArmComponent::name]).GetRightVector();
        const auto forwardDirection =XMVector3Normalize(XMVector3Cross(LookAtRightVector, GravityDirection));

        auto normalizeDirection = XMVector2Normalize(Direction);

        InputAcceration = (XMVectorGetX(normalizeDirection) * forwardDirection + XMVectorGetY(normalizeDirection) * LookAtRightVector)*maxAccAcceleration*2;
        bIsInputAcceration = true;
        return;
    }
}

// ��Ծ
void UCharacterMovementComponent::Jump() {
    Velocity += (-GravityDirection) * JumpInitialSpeed;
    Velocity = XMVector3Normalize(Velocity);
}

// ʩ������
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    // ����������ٶȵ��ٶ�������
    Velocity = DirectX::XMVectorAdd(Velocity, DirectX::XMVectorScale(GravityDirection, GravityAcceleration * DeltaTime));

    // ������������ϵ��ٶȴ�С
    float fallingSpeed = XMVectorGetX(DirectX::XMVector3Dot(Velocity, GravityDirection));

    // ����ٶȳ�����������ٶȣ���������
    if (fallingSpeed > MaxFallingSpeed) {
        Velocity -= GravityDirection * (fallingSpeed - MaxFallingSpeed);
    }
}

// ��ȡ��ǰ�ٶ�
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}

XMVECTOR UCharacterMovementComponent::GetGravityDirection()
{
    return GravityDirection;
}

void UCharacterMovementComponent::SetGravityDirection(XMVECTOR Direction)
{
    GravityDirection = Direction;
}


bool UCharacterMovementComponent::IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle) {
    float DotProduct = DirectX::XMVectorGetY(Normal);
    float SlopeAngle = acosf(DotProduct) * (180.0f / DirectX::XM_PI); // �����¶ȽǶ�
    return DotProduct > FloorDetectionThreshold && SlopeAngle <= MaxSlopeAngle;
}
// ����λ��
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {

    auto transform = Owner->GetTransform();
    transform.position = Velocity * DeltaTime + transform.position;
    Owner->SetTransform(transform);

    if (auto character = dynamic_cast<ACharacter*>(Owner)) {
        if (!bIsOnGround) {
            character->UpdateState(ACharacter::ECharacterState::Jumping);
        }
        else if (bIsOnGround && XMVectorGetX(XMVector3Length(Velocity))) {
            character->UpdateState(ACharacter::ECharacterState::Walking);
        }
        else if (XMVectorGetX(XMVector3Length(Velocity)) == 0) {
            character->UpdateState(ACharacter::ECharacterState::Idle);
        }
    }
}

void UCharacterMovementComponent::UpdateVelocity(float DeltaTime)
{
    // ȷ����ɫӵ���ߴ���
    if (!Owner) return;
    float currentSpeed = XMVectorGetX(XMVector3Length(Velocity));
    XMVECTOR normalizedVelocity = XMVector3Normalize(Velocity);

    Velocity += Acceleration * DeltaTime;
    // ȷ���ٶȲ������������ٶ�
    float newSpeed = XMVectorGetX(XMVector3Length(Velocity));
    if (newSpeed > maxGroundSpeed) {
        Velocity = XMVector3Normalize(Velocity) * maxGroundSpeed; // �����ٶ�
    }
    float deltaSpeed = XMVectorGetX(XMVector3Length(Acceleration * DeltaTime));
    if (currentSpeed - deltaSpeed < 0&&!bIsInputAcceration) {
        Velocity = { 0.f,0.f,0.f,0.f };
    }
    return;
    // ��ѯ��ײ��Ϣ
    auto World = Owner->GetWorld();
    if (!World) return;

    //����ڵ����ϣ��ȼ��㵱ǰ���ٶ�
    if (bIsOnGround) {
        // ��ȡ��ǰ�ٶȵĴ�С
        float currentSpeed = XMVectorGetX(XMVector3Length(Velocity));
        XMVECTOR normalizedVelocity = XMVector3Normalize(Velocity);

        if (bIsInputAcceration) {
            // �����������ٶȣ�����ݼ��ټ��ٶȵ����ٶ�
            Velocity += InputAcceration * DeltaTime;

            // �����ٶȲ������������ٶ�
            float newSpeed = XMVectorGetX(XMVector3Length(Velocity));
            if (newSpeed > maxGroundSpeed) {
                Velocity = normalizedVelocity * maxGroundSpeed;
            }
        }
        else {
            // ���û�����룬����м���
            float decelerationAmount = maxDeceleration * DeltaTime;
            currentSpeed -= decelerationAmount;

            // ����ٶȼ���0���£��򱣳�Ϊ��
            if (currentSpeed < 0.0f) {
                currentSpeed = 0.0f;
            }

            Velocity = normalizedVelocity * currentSpeed;
        }
    }

    //�ж���ײ��Ϣ
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

    }
    bIsOnGround = bOnGround;
}

void UCharacterMovementComponent::UpdateAcceleration(float DeltaTime)
{
    float currentSpeed = XMVectorGetX(DirectX::XMVector3Length(Velocity));
    Acceleration = { 0.f,0.f,0.f,0.f };
    if (currentSpeed > 0.0f)
    {
        // �����ٶȷ���ķ���������
        DirectX::XMVECTOR normalizedVelocity = DirectX::XMVector3Normalize(Velocity);

        // ���ü��ٶȵĴ�С�ͷ���ʹ�����ٶȷ����෴
        DirectX::XMVECTOR deceleration = -normalizedVelocity * maxDeceleration;

        // �����ٶ���ӵ���ǰ���ٶ�
        Acceleration += deceleration;
    }
    Acceleration += InputAcceration;
}

