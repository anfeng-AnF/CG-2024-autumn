#pragma once
#include <DirectXCollision.h>
#include <memory>
#include <string>
#include "Graphics.h"

class AActor;
class UActorComponent {
public:
    UActorComponent() {}
    virtual ~UActorComponent() {}

    // ��ʼ�����
    virtual void Initialize() {}

    // �������
    virtual void Update(float DeltaTime) = 0;

    // ��Ⱦ���
    virtual void Render(Graphics& gfx) {}

    // ���������� Actor
    void SetOwner(AActor* NewOwner);

    // ��ȡ������ Actor
    AActor* GetOwner() const;

protected:
    AActor* Owner; // ָ�������� Actor
};
