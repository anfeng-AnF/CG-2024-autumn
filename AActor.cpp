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
}

// ��Ⱦ����
void AActor::Render() {
    // ��Ⱦ�߼�
}

// ������ײ
void AActor::OnCollision() {
    // ��ײ�����߼�
}

// ������
void AActor::AddComponent(std::unique_ptr<UActorComponent> NewComponent) {
    if (!RootComponent) {
        RootComponent = std::move(NewComponent);
    }
    // �������չ����߼���֧�ֶ�����
}
