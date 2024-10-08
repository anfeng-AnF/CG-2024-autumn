#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Transform.h"
#include "UActorComponent.h"
#include "vector"

struct Component
{
    std::unique_ptr<UActorComponent> ActorComponent;
    std::vector<Component> ChildComponent;
};

class AActor {
public:
    AActor() {};
    virtual ~AActor() {};

    // ��ȡ�任
    const FTransform& GetTransform() const;

    // ���ñ任
    void SetTransform(const FTransform& NewTransform);

    // ÿ֡����
    virtual void Tick(float DeltaTime);

    // ��Ⱦ���������Ը��ǣ�
    virtual void Render();

    // ������ײ�������Ҫ��
    virtual void OnCollision();

    // ������
    void AddComponent(std::unique_ptr<UActorComponent> NewComponent);
protected:

    FTransform Transform; // �任
    std::unique_ptr<Component> RootComponent;
};