#pragma once
#include "SkeletonMesh.h"
#include "UAnimationComponent.h"
#include "UActorComponent.h"

class USkeletalMeshComponent : public UActorComponent {
public:
    USkeletalMeshComponent();
    ~USkeletalMeshComponent();

    // ���ù���������
    void SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh);

    // ��ȡ����������
    std::shared_ptr<SkeletonMesh> GetSkeletalMesh() const;

    // ��Ӷ������
    void AddAnimationComponent(std::shared_ptr<UAnimationComponent> AnimComp);

    // ����������������ȣ�
    void Update();

    // ��Ⱦ����
    void Render();

private:
    std::shared_ptr<SkeletonMesh> SkeletalMesh; // ����������
    std::vector<std::shared_ptr<UAnimationComponent>> AnimationComponents; // �������
    DirectX::XMMATRIX Transform; // �任����
};