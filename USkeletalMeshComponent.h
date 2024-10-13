#pragma once
#include "SkeletonMesh.h"
#include "UActorComponent.h"
#include "AnimInstance.h"

class USkeletalMeshComponent : public UActorComponent {
    using UAnimInstance = AnimInstance;
public:
    USkeletalMeshComponent();
    ~USkeletalMeshComponent();

    // 设置骨骼网格体
    void SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh);

    // 获取骨骼网格体
    std::shared_ptr<SkeletonMesh> GetSkeletalMesh() const;

    // 添加动画组件
    void SetAnimationInstance(std::shared_ptr<UAnimInstance> pAnimInstance);
    std::shared_ptr<UAnimInstance> GetAnimationInstance();

    // 更新组件（处理动画等）
    void Update(float DeltaTime)override;

    // 渲染函数
    void Render(Graphics& gfx)override;
public:
    const static std::string name;
private:
    std::shared_ptr<SkeletonMesh> pSkeletalMesh; // 骨骼网格体
    std::shared_ptr<UAnimInstance> pAnimInstance; // 动画组件
    DirectX::XMMATRIX Transform; // 变换矩阵
};