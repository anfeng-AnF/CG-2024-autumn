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
    void Update(const FTransform& SpringArmTransform);

    // ��ȡ��ǰ�任
    FTransform GetCurrentTransform() const;

    void SetCamera(Camera* cam);
private:
    Camera* cam;
    float FieldOfView; // ��Ұ
    FTransform CurrentTransform; // ��ǰ�任
};