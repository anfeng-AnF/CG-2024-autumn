#pragma once
#include <DirectXCollision.h>
#include <memory>
#include "AActor.h"

class UActorComponent {
public:
    UActorComponent() {}
    virtual ~UActorComponent() {}

    // ��ʼ�����
    virtual void Initialize() {}

    // �������
    virtual void Update(float DeltaTime) = 0;

    // ��Ⱦ���
    virtual void Render() {}

    // ���������� Actor
    void SetOwner(AActor* NewOwner);

    // ��ȡ������ Actor
    std::shared_ptr<AActor> GetOwner() const;

protected:
    std::shared_ptr<AActor> Owner; // ָ�������� Actor
};
