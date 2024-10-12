#pragma once
#include "AnimAssert.h"

class AnimInstance {
    friend class UAnimationComponent;
public:
    AnimInstance(SkeletonMesh& mesh);
    void Update(float deltaTime);
    void SetCurrentAnim(AnimAsset Anim);
protected:
    SkeletonMesh& mesh;
    AnimAsset currentAnim;
    std::unordered_map<std::string, dx::XMMATRIX> currentPose;
    bool loopPlay = false;
    //StateMachine stateMachine;
    float currentTime = 0.0f;
};