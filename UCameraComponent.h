#pragma once
#include "Camera.h"
#include "UActorComponent.h"

class UCameraComponent : public UActorComponent {
public:
    UCameraComponent();
    UCameraComponent(std::shared_ptr<Camera> cam);
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

    void SetCamera(std::shared_ptr<Camera> cam);

    XMMATRIX GetMatrix();
public:
    const static std::string name;
private:
    std::shared_ptr<Camera> cam;
    float FieldOfView; // 视野
    FTransform CurrentTransform; // 当前变换
};