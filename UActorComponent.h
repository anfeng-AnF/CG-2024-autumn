#pragma once
#include <DirectXCollision.h>
#include <memory>
#include <string>
#include "Graphics.h"
#include "Transform.h"
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

    virtual DirectX::XMMATRIX GetRelationTransformMatrix();

    // ��ȡ������ Actor
    AActor* GetOwner() const;

    FTransform RelationTransform;
protected:
    AActor* Owner; // ָ�������� Actor
};
