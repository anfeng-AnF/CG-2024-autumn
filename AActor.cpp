#include "AActor.h"

// ��ȡ�任
const FTransform& AActor::GetTransform() const {
    return Transform;
}

// ���ñ任
void AActor::SetTransform(const FTransform& NewTransform) {
    Transform = NewTransform;
}

// ÿ֡����
void AActor::Tick(float DeltaTime) {
    // �����߼��������ƶ�����ת��



    //process Components
    ProcessComponentTree(DeltaTime);
}

// ��Ⱦ����
void AActor::Render() {
    for (auto& comp : Components) {
        comp.second->ActorComponent->Render();
    }
}

// ������ײ
void AActor::OnCollision() {
    // ��ײ�����߼�
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


