#include "USkeletalMeshComponent.h"

// ��̬��Ա������ʼ��
const std::string USkeletalMeshComponent::name = "USkeletalMeshComponent";

// ���캯��
USkeletalMeshComponent::USkeletalMeshComponent()
{
    // ��ʵ��
}

// ��������
USkeletalMeshComponent::~USkeletalMeshComponent()
{
    // ��ʵ��
}

// ���ù���������
void USkeletalMeshComponent::SetSkeletalMesh(std::shared_ptr<SkeletonMesh> Mesh)
{
    pSkeletalMesh = Mesh;
}

// ��ȡ����������
std::shared_ptr<SkeletonMesh> USkeletalMeshComponent::GetSkeletalMesh() const
{
    return pSkeletalMesh;
}

void USkeletalMeshComponent::SetAnimationInstance(std::shared_ptr<UAnimInstance> pAnimInstance)
{
    if(pAnimInstance)
    this->pAnimInstance = pAnimInstance;
}

void USkeletalMeshComponent::Update(float DeltaTime)
{
    if(pAnimInstance)
    pAnimInstance->Update(DeltaTime);
}

// ��Ⱦ����
void USkeletalMeshComponent::Render(Graphics& gfx)
{
    if(pSkeletalMesh)
    this->pSkeletalMesh->Draw(gfx,this->Transform);
}
