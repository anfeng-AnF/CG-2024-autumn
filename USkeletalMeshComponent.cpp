#include "USkeletalMeshComponent.h"
#include "AActor.h"
// 静态成员变量初始化
const std::string USkeletalMeshComponent::name = "USkeletalMeshComponent";

// 构造函数
USkeletalMeshComponent::USkeletalMeshComponent()
{
    // 空实现
}

// 析构函数
USkeletalMeshComponent::~USkeletalMeshComponent()
{
    // 空实现
}

// 设置骨骼网格体
void USkeletalMeshComponent::SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh)
{
    pSkeletalMesh = Mesh;
}

// 获取骨骼网格体
std::shared_ptr<SkeletonMesh> USkeletalMeshComponent::GetSkeletalMesh() const
{
    return pSkeletalMesh;
}

void USkeletalMeshComponent::SetAnimationInstance(std::shared_ptr<UAnimInstance> pAnimInstance)
{
    if(pAnimInstance)
    this->pAnimInstance = pAnimInstance;
}

std::shared_ptr<AnimInstance> USkeletalMeshComponent::GetAnimationInstance()
{
    return this->pAnimInstance;
}

void USkeletalMeshComponent::Update(float DeltaTime)
{
    if (pAnimInstance) {
        pAnimInstance->Update(DeltaTime);
    }
    this->Transform = Owner->GetComponentFinalTransform()[USkeletalMeshComponent::name];
}

// 渲染函数
void USkeletalMeshComponent::Render(Graphics& gfx)
{
    if (pSkeletalMesh) {
        this->pSkeletalMesh->Draw(gfx, this->Transform);
    }
}
