#pragma once
#include "Camera.h"
#include "UActorComponent.h"

class UCameraComponent : public UActorComponent {
public:
    UCameraComponent();
    UCameraComponent(Camera* cam);
    ~UCameraComponent();

    // 设置视野（FOV）
    void SetFieldOfView(float FOV);

    // 获取视野
    float GetFieldOfView() const;

    // 更新相机位置
    void Update(float DeltaTime)override;

    void SetTransform(const FTransform& transform);

    // 获取当前变换
    FTransform GetCurrentTransform() const;

    void SetCamera(Camera* cam);

public:
    const static std::string name;
private:
    Camera* cam;
    float FieldOfView; // 视野
    FTransform CurrentTransform; // 当前变换
};