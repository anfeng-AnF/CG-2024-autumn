#pragma once
#include "SkeletonMesh.h"
#include "UAnimationComponent.h"
#include "UActorComponent.h"

class USkeletalMeshComponent : public UActorComponent {
public:
    USkeletalMeshComponent();
    ~USkeletalMeshComponent();

    // 设置骨骼网格体
    void SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh);

    // 获取骨骼网格体
    std::shared_ptr<SkeletonMesh> GetSkeletalMesh() const;

    // 添加动画组件
    void AddAnimationComponent(std::shared_ptr<UAnimationComponent> AnimComp);

    // 更新组件（处理动画等）
    void Update();

    // 渲染函数
    void Render();

private:
    std::shared_ptr<SkeletonMesh> SkeletalMesh; // 骨骼网格体
    std::vector<std::shared_ptr<UAnimationComponent>> AnimationComponents; // 动画组件
    DirectX::XMMATRIX Transform; // 变换矩阵
};