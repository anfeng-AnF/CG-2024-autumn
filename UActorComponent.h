#pragma once

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
    virtual void Render() {}

    // ���������� Actor
    void SetOwner(AActor* NewOwner);

    // ��ȡ������ Actor
    AActor* GetOwner() const;

protected:
    AActor* Owner; // ָ�������� Actor
};
