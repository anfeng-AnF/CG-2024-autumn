#include "UStaticMeshComponent.h"
#include "AActor.h"
// 静态成员变量初始化
const std::string UStaticMeshComponent::name = "UStaticMeshComponent";

// 构造函数
UStaticMeshComponent::UStaticMeshComponent():
    Transform(DirectX::XMMatrixIdentity())
{
    // 空实现
}

// 析构函数
UStaticMeshComponent::~UStaticMeshComponent()
{
    // 空实现
}

// 设置网格体
void UStaticMeshComponent::SetStaticMesh(std::shared_ptr<UStaticMesh> Mesh)
{
    this->StaticMesh = Mesh;
}

// 获取网格体
std::shared_ptr<UStaticMeshComponent::UStaticMesh> UStaticMeshComponent::GetStaticMesh() const
{
    return this->StaticMesh;
}

// 更新组件（可能用于计算渲染状态等）
void UStaticMeshComponent::Update(float DeltaTime)
{
    this->Transform = Owner->GetComponentFinalTransform()[UStaticMeshComponent::name];
}

// 渲染函数
void UStaticMeshComponent::Render(Graphics& gfx)
{
    this->StaticMesh->Draw(gfx,Transform);
}
