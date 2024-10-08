#pragma once
#include "Mesh.h"
#include "UActorComponent.h"

class UStaticMeshComponent : public UActorComponent {
    using UStaticMesh = Model;
public:
    UStaticMeshComponent();
    ~UStaticMeshComponent();

    // 设置网格体
    void SetStaticMesh(std::shared_ptr<UStaticMesh> Mesh);

    // 获取网格体
    std::shared_ptr<UStaticMesh> GetStaticMesh() const;

    // 更新组件（可能用于计算渲染状态等）
    void Update();

    // 渲染函数
    void Render();

private:
    std::shared_ptr<UStaticMesh> StaticMesh; // 静态网格体
    DirectX::XMMATRIX Transform; // 变换矩阵
};