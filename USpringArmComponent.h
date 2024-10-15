#pragma once
#include "Transform.h"
#include "UActorComponent.h"

class USpringArmComponent : public UActorComponent {
public:
    USpringArmComponent();
    ~USpringArmComponent();

    void SetArmLength(float Length);

    float GetArmLength() const;

    virtual void Update(const FTransform& OwnerTransform);

    FTransform GetCurrentTransform() const;

    virtual DirectX::XMMATRIX GetRelationTransformMatrix()override;

public:
    const static std::string name;
private:
    float ArmLength;
    FTransform CurrentTransform;

    // ͨ�� UActorComponent �̳�
    void Update(float DeltaTime) override;
};