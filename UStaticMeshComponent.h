#pragma once
#include "Mesh.h"
#include "UActorComponent.h"

class UStaticMeshComponent : public UActorComponent {
    using UStaticMesh = Model;
public:
    UStaticMeshComponent();
    ~UStaticMeshComponent();

    // ����������
    void SetStaticMesh(std::shared_ptr<UStaticMesh> Mesh);

    // ��ȡ������
    std::shared_ptr<UStaticMesh> GetStaticMesh() const;

    // ����������������ڼ�����Ⱦ״̬�ȣ�
    void Update();

    // ��Ⱦ����
    void Render();

private:
    std::shared_ptr<UStaticMesh> StaticMesh; // ��̬������
    DirectX::XMMATRIX Transform; // �任����
};