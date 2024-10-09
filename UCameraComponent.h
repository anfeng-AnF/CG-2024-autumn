#pragma once
#include "Camera.h"
#include "UActorComponent.h"

class UCameraComponent : public UActorComponent {
public:
    UCameraComponent();
    UCameraComponent(Camera* cam);
    ~UCameraComponent();

    // ������Ұ��FOV��
    void SetFieldOfView(float FOV);

    // ��ȡ��Ұ
    float GetFieldOfView() const;

    // �������λ��
    void Update(float DeltaTime)override;

    void SetTransform(const FTransform& transform);

    // ��ȡ��ǰ�任
    FTransform GetCurrentTransform() const;

    void SetCamera(Camera* cam);

public:
    const static std::string name;
private:
    Camera* cam;
    float FieldOfView; // ��Ұ
    FTransform CurrentTransform; // ��ǰ�任
};