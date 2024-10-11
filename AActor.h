#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "Transform.h"
#include "UActorComponent.h"
#include "vector"
#include "World.h"
#include "Window.h"

struct Component
{
    std::shared_ptr<UActorComponent> ActorComponent;
    std::vector<std::string> ChildComponent;
    FTransform RelateTransfom;
    Component()=default;
    Component(std::shared_ptr<UActorComponent> ActorComponent) :ActorComponent(std::move(ActorComponent)) {};
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
    void AddComponent(std::string name, std::shared_ptr<UActorComponent> NewComponent,std::string ComponentName);

    std::shared_ptr<UWorld> GetWorld();

    std::unordered_map<std::string, DirectX::XMMATRIX>& GetComponentFinalTransform();
protected:
    std::shared_ptr<UWorld> World;
    FTransform Transform; // �任
    std::unordered_map<std::string, std::shared_ptr<Component>> Components{std::make_pair("Root",nullptr)};
    std::unordered_map<std::string, DirectX::XMMATRIX> ComponentFinalTransform;

private:
    void ProcessComponentTree(float DeltaTime);
    void ProcessComponentTreeDfs(const std::string& NodeName,const DirectX::XMMATRIX& ParentTransform);
};