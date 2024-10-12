#pragma once
#include "Transform.h"
#include "UActorComponent.h"

class USpringArmComponent : public UActorComponent {
public:
    USpringArmComponent();
    ~USpringArmComponent();

    void SetArmLength(float Length);

    float GetArmLength() const;

    void Update(const FTransform& OwnerTransform);

    FTransform GetCurrentTransform() const;
public:
    const static std::string name;
private:
    float ArmLength;
    FTransform CurrentTransform;

    // Í¨¹ý UActorComponent ¼Ì³Ð
    void Update(float DeltaTime) override;
};