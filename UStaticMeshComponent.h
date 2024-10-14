#pragma once
#include "Mesh.h"
#include "UActorComponent.h"

class UStaticMeshComponent : public UActorComponent {
    using UStaticMesh = StaticMesh;
public:
    UStaticMeshComponent();
    ~UStaticMeshComponent();

    // ����������
    void SetStaticMesh(std::shared_ptr<UStaticMesh> Mesh);

    // ��ȡ������
    std::shared_ptr<UStaticMesh> GetStaticMesh() const;

    // ����������������ڼ�����Ⱦ״̬�ȣ�
    void Update(float DeltaTime)override;

    // ��Ⱦ����
    void Render(Graphics& gfx)override;
public:
    const static std::string name;
private:
    std::shared_ptr<UStaticMesh> StaticMesh; // ��̬������
    DirectX::XMMATRIX Transform; // �任����
};