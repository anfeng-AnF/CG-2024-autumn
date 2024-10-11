#include "AActor.h"

// 获取变换
const FTransform& AActor::GetTransform() const {
    return Transform;
}

// 设置变换
void AActor::SetTransform(const FTransform& NewTransform) {
    Transform = NewTransform;
}

// 每帧更新
void AActor::Tick(float DeltaTime) {
    // 更新逻辑，例如移动、旋转等



    //process Components
    ProcessComponentTree(DeltaTime);
}

// 渲染函数
void AActor::Render() {
    for (auto& comp : Components) {
        comp.second->ActorComponent->Render();
    }
}

// 处理碰撞
void AActor::OnCollision() {
    // 碰撞处理逻辑
}

void AActor::AddComponent(std::string AttachName, std::shared_ptr<UActorComponent> NewComponent, std::string ComponentName)
{
    if (Components.find("Root") == Components.end()) {
        Components["Root"] = std::make_shared<Component>(NewComponent);
        Components["Root"]->ActorComponent->SetOwner(this);
    }
    if (Components.find(AttachName) != Components.end()) {
        std::shared_ptr<Component> NewCompnt=std::make_shared<Component>(std::move(NewComponent));
        NewCompnt->ActorComponent->SetOwner(this);
        Components[ComponentName] = NewCompnt;
        Components[AttachName]->ChildComponent.push_back(ComponentName);
    }
}

std::shared_ptr<UWorld> AActor::GetWorld()
{
    return World;
}

std::unordered_map<std::string, DirectX::XMMATRIX>& AActor::GetComponentFinalTransform()
{
    return ComponentFinalTransform;
}

void AActor::ProcessComponentTree(float DeltaTime)
{
    ComponentFinalTransform["Root"] = Transform.GetMatrix();
    ProcessComponentTreeDfs("Root", ComponentFinalTransform["Root"]);
    for (auto& comp : Components) {
        comp.second->ActorComponent->Update(DeltaTime);
    }
}

void AActor::ProcessComponentTreeDfs(const std::string& NodeName, const DirectX::XMMATRIX& ParentTransform)
{
    auto it = Components.find(NodeName);
    if (it == Components.end()) {
        return;
    }

    auto CurrentComponent = it->second;

    ComponentFinalTransform[NodeName] = CurrentComponent->RelateTransfom.GetMatrix() * ParentTransform;

    for (const auto& childName : CurrentComponent->ChildComponent) {
        ProcessComponentTreeDfs(childName, ComponentFinalTransform[NodeName]);
    }
}


