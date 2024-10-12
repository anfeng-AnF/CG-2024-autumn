#include "AActor.h"
#include "UActorComponent.h"
#include "World.h"
const std::string AActor::RootComponentName="Root";

AActor::AActor()
{
}

AActor::~AActor()
{

}

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

void AActor::Render(Graphics& Gfx)
{
    for (auto& comp : Components) {
        comp.second->ActorComponent->Render(Gfx);
    }
}


// 处理碰撞
void AActor::OnCollision() {
    // 碰撞处理逻辑
}

void AActor::AddComponent(std::string AttachName, std::shared_ptr<UActorComponent> NewComponent, std::string ComponentName)
{
    if (Components.find(AActor::RootComponentName) == Components.end()) {
        Components[AActor::RootComponentName] = std::make_shared<Component>(NewComponent);
        Components[AActor::RootComponentName]->ActorComponent->SetOwner(this);
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

std::shared_ptr<Component> AActor::GetComponentInfo(std::string ComponentName)
{
    if (Components.find(ComponentName) != Components.end()) {
        return Components[ComponentName];
    }
    else
    {
        return nullptr;
    }
}

void AActor::SetWorld(std::shared_ptr<UWorld> World)
{
    this->World = World;
}

void AActor::ProcessComponentTree(float DeltaTime)
{
    ComponentFinalTransform[AActor::RootComponentName] = Transform.GetMatrix();
    ProcessComponentTreeDfs(AActor::RootComponentName, ComponentFinalTransform[AActor::RootComponentName]);
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

    ComponentFinalTransform[NodeName] = CurrentComponent->ActorComponent->GetRelationTransformMatrix()* ParentTransform;

    for (const auto& childName : CurrentComponent->ChildComponent) {
        ProcessComponentTreeDfs(childName, ComponentFinalTransform[NodeName]);
    }
}


