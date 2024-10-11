#include "UCharacterMovementComponent.h"
#include <DirectXMathVector.inl>

const std::string UCharacterMovementComponent::name = "UCharacterMovementComponent";

// 构造函数
UCharacterMovementComponent::UCharacterMovementComponent()
    : Velocity(XMVectorZero()), // 初始化速度为零向量
    MovementInput(XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f)), // 初始化移动输入为零
    JumpHeight(300.0f), // 设置跳跃高度
    bIsOnGround(true), // 默认在地面上
    GravityScale(9.81f) // 设置重力加速度
{}

// 析构函数
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// 每帧更新
void UCharacterMovementComponent::Update(float DeltaTime) {
    // 计算物理速度，含碰撞
    UpdateVelocity(DeltaTime);

    // 更新位置
    UpdatePosition(DeltaTime);
}

// 设置移动输入
void UCharacterMovementComponent::SetMovementInput(const XMFLOAT3& Direction) {
    MovementInput = XMVectorSet(Direction.x, Direction.y, Direction.z, 0.0f);
}

// 跳跃
void UCharacterMovementComponent::Jump() {
    Velocity += (-GravityDirection) * JumpHeight;
    Velocity = XMVector3Normalize(Velocity);
}

// 施加重力
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    Velocity = DirectX::XMVectorAdd(Velocity, DirectX::XMVectorScale(GravityDirection, GravityScale));
}

// 获取当前速度
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}


bool UCharacterMovementComponent::IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle) {
    float DotProduct = DirectX::XMVectorGetY(Normal);
    float SlopeAngle = acosf(DotProduct) * (180.0f / DirectX::XM_PI); // 计算坡度角度
    return DotProduct > FloorDetectionThreshold && SlopeAngle <= MaxSlopeAngle;
}
// 更新位置
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {
    auto transform = Owner->GetTransform();
    transform.position += Velocity * Speed * DeltaTime + transform.position;
    Owner->SetTransform(transform);
}

void UCharacterMovementComponent::UpdateVelocity(float DeltaTime)
{
    // 确保角色拥有者存在
    if (!Owner) return;

    // 查询碰撞信息
    auto World = Owner->GetWorld();
    if (!World) return;

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

        // 添加重力影响
        Velocity += GravityDirection * GravityScale * DeltaTime;
    }
    Velocity = XMVector3Normalize(Velocity);
}

