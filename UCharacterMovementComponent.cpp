#include "UCharacterMovementComponent.h"
#include <DirectXMathVector.inl>
#include "World.h"
#include "ACharacter.h"
#include "USpringArmComponent.h"
const std::string UCharacterMovementComponent::name = "UCharacterMovementComponent";

UCharacterMovementComponent::UCharacterMovementComponent(AActor* Owner)
    : Velocity(XMVectorZero()), // 初始化速度为零向量
    JumpInitialSpeed(300.0f), // 设置跳跃高度
    bIsOnGround(true), // 默认在地面上
    GravityAcceleration(9.81f), // 设置重力加速度
    GravityDirection({ 0.f,-1.f,0.f,0.f }),
    Acceleration({ 0.f,0.f,0.f,0.f }),
    InputAcceration({ 0.f,0.f,0.f,0.f })
{
    this->Owner = Owner;
}

// 析构函数
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// 每帧更新
void UCharacterMovementComponent::Update(float DeltaTime) {
    //计算加速度
    UpdateAcceleration(DeltaTime);

    // 计算物理速度，含碰撞
    UpdateVelocity(DeltaTime);

    // 更新位置
    UpdatePosition(DeltaTime);
}

// 设置移动输入
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

// 跳跃
void UCharacterMovementComponent::Jump() {
    Velocity += (-GravityDirection) * JumpInitialSpeed;
    Velocity = XMVector3Normalize(Velocity);
}

// 施加重力
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    // 添加重力加速度到速度向量中
    Velocity = DirectX::XMVectorAdd(Velocity, DirectX::XMVectorScale(GravityDirection, GravityAcceleration * DeltaTime));

    // 检查重力方向上的速度大小
    float fallingSpeed = XMVectorGetX(DirectX::XMVector3Dot(Velocity, GravityDirection));

    // 如果速度超过最大下落速度，则限制它
    if (fallingSpeed > MaxFallingSpeed) {
        Velocity -= GravityDirection * (fallingSpeed - MaxFallingSpeed);
    }
}

// 获取当前速度
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
    float SlopeAngle = acosf(DotProduct) * (180.0f / DirectX::XM_PI); // 计算坡度角度
    return DotProduct > FloorDetectionThreshold && SlopeAngle <= MaxSlopeAngle;
}
// 更新位置
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
    // 确保角色拥有者存在
    if (!Owner) return;
    float currentSpeed = XMVectorGetX(XMVector3Length(Velocity));
    XMVECTOR normalizedVelocity = XMVector3Normalize(Velocity);

    Velocity += Acceleration * DeltaTime;
    // 确保速度不超过最大地面速度
    float newSpeed = XMVectorGetX(XMVector3Length(Velocity));
    if (newSpeed > maxGroundSpeed) {
        Velocity = XMVector3Normalize(Velocity) * maxGroundSpeed; // 限制速度
    }
    float deltaSpeed = XMVectorGetX(XMVector3Length(Acceleration * DeltaTime));
    if (currentSpeed - deltaSpeed < 0&&!bIsInputAcceration) {
        Velocity = { 0.f,0.f,0.f,0.f };
    }
    return;
    // 查询碰撞信息
    auto World = Owner->GetWorld();
    if (!World) return;

    //如果在地面上，先计算当前加速度
    if (bIsOnGround) {
        // 获取当前速度的大小
        float currentSpeed = XMVectorGetX(XMVector3Length(Velocity));
        XMVECTOR normalizedVelocity = XMVector3Normalize(Velocity);

        if (bIsInputAcceration) {
            // 如果有输入加速度，则根据加速加速度调整速度
            Velocity += InputAcceration * DeltaTime;

            // 限制速度不超过最大地面速度
            float newSpeed = XMVectorGetX(XMVector3Length(Velocity));
            if (newSpeed > maxGroundSpeed) {
                Velocity = normalizedVelocity * maxGroundSpeed;
            }
        }
        else {
            // 如果没有输入，则进行减速
            float decelerationAmount = maxDeceleration * DeltaTime;
            currentSpeed -= decelerationAmount;

            // 如果速度减到0以下，则保持为零
            if (currentSpeed < 0.0f) {
                currentSpeed = 0.0f;
            }

            Velocity = normalizedVelocity * currentSpeed;
        }
    }

    //判断碰撞信息
    std::vector<FCollisionResult> CollisionResults = World->QuaryCollision(Owner);

    // 如果没有发生碰撞，则进入下落状态
    if (CollisionResults.empty()) {
        // 应用重力更新速度
        ApplyGravity(DeltaTime);
        return;
    }

    DirectX::XMVECTOR GroundNormal = DirectX::XMVectorZero();
    DirectX::XMVECTOR WallNormal = DirectX::XMVectorZero();
    bool bOnGround = false;

    // 遍历所有碰撞结果，区分地面和墙面
    for (const auto& CollisionResult : CollisionResults) {
        DirectX::XMVECTOR Normal = XMVectorSet(CollisionResult.HitResult.Normal.x, CollisionResult.HitResult.Normal.y, CollisionResult.HitResult.Normal.z, 0.0f);

        if (IsWalkableSurface(Normal, MaxSlopeAngle)) {
            // 如果是可行走的地面，累加地面法线并标记为在地面上
            GroundNormal = DirectX::XMVectorAdd(GroundNormal, Normal);
            bOnGround = true;
        }
        else {
            // 如果是墙面，累加墙面法线
            WallNormal = DirectX::XMVectorAdd(WallNormal, Normal);
        }
    }
    // 如果在地面上，停止下落并移除速度中沿重力方向的分量
    if (bOnGround) {
        // 归一化地面法线
        GroundNormal = DirectX::XMVector3Normalize(GroundNormal);

        // 将速度中沿着重力方向的分量设置为零（停止下落）
        DirectX::XMVECTOR GravityVelocity = DirectX::XMVector3Dot(Velocity, GravityDirection) * GravityDirection;
        Velocity -= GravityVelocity;
    }
    else {
        // 如果不在地面上，将所有速度在墙面法线上的分量移除
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
        // 计算速度方向的反方向向量
        DirectX::XMVECTOR normalizedVelocity = DirectX::XMVector3Normalize(Velocity);

        // 设置减速度的大小和方向，使其与速度方向相反
        DirectX::XMVECTOR deceleration = -normalizedVelocity * maxDeceleration;

        // 将减速度添加到当前加速度
        Acceleration += deceleration;
    }
    Acceleration += InputAcceration;
}

