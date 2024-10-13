#pragma once
#include "SkeletonMesh.h"
#include "UActorComponent.h"
#include "AnimInstance.h"

class USkeletalMeshComponent : public UActorComponent {
    using UAnimInstance = AnimInstance;
public:
    USkeletalMeshComponent();
    ~USkeletalMeshComponent();

    // ���ù���������
    void SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh);

    // ��ȡ����������
    std::shared_ptr<SkeletonMesh> GetSkeletalMesh() const;

    // ��Ӷ������
    void SetAnimationInstance(std::shared_ptr<UAnimInstance> pAnimInstance);
    std::shared_ptr<UAnimInstance> GetAnimationInstance();

    // ����������������ȣ�
    void Update(float DeltaTime)override;

    // ��Ⱦ����
    void Render(Graphics& gfx)override;
public:
    const static std::string name;
private:
    std::shared_ptr<SkeletonMesh> pSkeletalMesh; // ����������
    std::shared_ptr<UAnimInstance> pAnimInstance; // �������
    DirectX::XMMATRIX Transform; // �任����
};