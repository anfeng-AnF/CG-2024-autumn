#include "UStaticMeshComponent.h"

// ��̬��Ա������ʼ��
const std::string UStaticMeshComponent::name = "UStaticMeshComponent";

// ���캯��
UStaticMeshComponent::UStaticMeshComponent()
{
    // ��ʵ��
}

// ��������
UStaticMeshComponent::~UStaticMeshComponent()
{
    // ��ʵ��
}

// ����������
void UStaticMeshComponent::SetStaticMesh(std::shared_ptr<UStaticMesh> Mesh)
{
    // ��ʵ��
}

// ��ȡ������
std::shared_ptr<UStaticMeshComponent::UStaticMesh> UStaticMeshComponent::GetStaticMesh() const
{
    // ��ʵ�֣����� nullptr
    return nullptr;
}

// ����������������ڼ�����Ⱦ״̬�ȣ�
void UStaticMeshComponent::Update(float DeltaTime)
{
    // ��ʵ��
}

// ��Ⱦ����
void UStaticMeshComponent::Render(Graphics& gfx)
{
    // ��ʵ��
}
