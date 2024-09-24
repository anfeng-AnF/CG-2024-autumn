#pragma once
#include "AnimAssert.h"

class AnimInstance {
public:
    AnimInstance(SkeletonMesh& mesh);

    SkeletonMesh& mesh;
    AnimAsset currentAnim;

    bool loopPlay = false;
    //StateMachine stateMachine;
    float currentTime = 0.0f;
    void Update(float deltaTime);
};