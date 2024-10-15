#include "UStaticMeshComponent.h"
#include "AActor.h"
// ��̬��Ա������ʼ��
const std::string UStaticMeshComponent::name = "UStaticMeshComponent";

// ���캯��
UStaticMeshComponent::UStaticMeshComponent():
    Transform(DirectX::XMMatrixIdentity())
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
    this->StaticMesh = Mesh;
}

// ��ȡ������
std::shared_ptr<UStaticMeshComponent::UStaticMesh> UStaticMeshComponent::GetStaticMesh() const
{
    return this->StaticMesh;
}

// ����������������ڼ�����Ⱦ״̬�ȣ�
void UStaticMeshComponent::Update(float DeltaTime)
{
    this->Transform = Owner->GetComponentFinalTransform()[UStaticMeshComponent::name];
}

// ��Ⱦ����
void UStaticMeshComponent::Render(Graphics& gfx)
{
    this->StaticMesh->Draw(gfx,Transform);
}
