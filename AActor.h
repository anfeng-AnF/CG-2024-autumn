#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "Transform.h"
#include "vector"
#include "Window.h"
#include "Graphics.h"

class UActorComponent;
class UWorld;

struct Component
{
    std::shared_ptr<UActorComponent> ActorComponent;
    std::vector<std::string> ChildComponent;
    FTransform RelateTransfom;
    Component()=default;
    Component(std::shared_ptr<UActorComponent> ActorComponent) :ActorComponent(std::move(ActorComponent)) {};
};

class AActor 
{
public:
    AActor();
    virtual ~AActor();

    // 获取变换
    const FTransform& GetTransform() const;

    // 设置变换
    void SetTransform(const FTransform& NewTransform);

    // 每帧更新
    virtual void Tick(float DeltaTime);

    // 渲染函数（可以覆盖）
    virtual void Render(Graphics&Gfx);

    // 处理碰撞（如果需要）
    virtual void OnCollision();

    // 添加组件
    void AddComponent(std::string name, std::shared_ptr<UActorComponent> NewComponent,std::string ComponentName);

    std::shared_ptr<UWorld> GetWorld();

    std::unordered_map<std::string, DirectX::XMMATRIX>& GetComponentFinalTransform();

    std::shared_ptr<Component> GetComponentInfo(std::string ComponentName);
public:
    const static std::string RootComponentName;
protected:
    std::shared_ptr<UWorld> World;
    FTransform Transform; // 变换
    std::unordered_map<std::string, std::shared_ptr<Component>> Components;
    std::unordered_map<std::string, DirectX::XMMATRIX> ComponentFinalTransform;

private:
    void ProcessComponentTree(float DeltaTime);
    void ProcessComponentTreeDfs(const std::string& NodeName,const DirectX::XMMATRIX& ParentTransform);
};