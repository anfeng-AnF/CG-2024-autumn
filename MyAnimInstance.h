#pragma once
#include "AnimInstance.h"

class MyAnimInstance :public AnimInstance
{
	typedef AnimInstance Super;
    struct AnimInfo
    {
        float CurrentGroundSpeed;
        float MaxGroundSpeed;
    };
public:
	MyAnimInstance(SkeletonMesh& SkMesh);
	void Update(float DeltaTime)override;
    void SetAnimInfo(AnimInfo Speed);
protected:
    enum State 
    {
        Idle,
        Run,
        BeginRun,
        Deceleration
    };
	State currentState = Idle;
    AnimInfo Info;
};