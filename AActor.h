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

    // 获取变换
    const FTransform& GetTransform() const;

    // 设置变换
    void SetTransform(const FTransform& NewTransform);

    // 每帧更新
    virtual void Tick(float DeltaTime);

    // 渲染函数（可以覆盖）
    virtual void Render();

    // 处理碰撞（如果需要）
    virtual void OnCollision();

    // 添加组件
    void AddComponent(std::string name, std::shared_ptr<UActorComponent> NewComponent,std::string ComponentName);

    std::shared_ptr<UWorld> GetWorld();

    std::unordered_map<std::string, DirectX::XMMATRIX>& GetComponentFinalTransform();
protected:
    std::shared_ptr<UWorld> World;
    FTransform Transform; // 变换
    std::unordered_map<std::string, std::shared_ptr<Component>> Components{std::make_pair("Root",nullptr)};
    std::unordered_map<std::string, DirectX::XMMATRIX> ComponentFinalTransform;

private:
    void ProcessComponentTree(float DeltaTime);
    void ProcessComponentTreeDfs(const std::string& NodeName,const DirectX::XMMATRIX& ParentTransform);
};