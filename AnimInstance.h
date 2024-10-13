#pragma once
#include "AnimAssert.h"
#include <functional>

class AnimStateMachine {
public:
    void AddState(const std::string& stateName, std::shared_ptr<AnimAsset> animation);

    void SetTransitionCondition(const std::string& fromState, const std::string& toState, std::function<bool()> condition,float TransitionTime=1.0f);

    void Update(float deltaTime);

    std::unordered_map<std::string, dx::XMMATRIX> GetCurrentPose();

    void SetCurrentState(std::string curState);
protected:
    void UpdateTransition(float deltaTime);

private:
    struct State {
        std::shared_ptr<AnimAsset> animation;
        std::unordered_map<std::string,std::pair<std::function<bool()>,float>> transitions; // 过渡条件
    };

    std::unordered_map<std::string, dx::XMMATRIX> CurrentPose;
    std::unordered_map<std::string, State> states;                  // 所有的状态
    std::string currentState;                                       // 当前状态
    float AnimTime = 0.0f;                                          // 当前动画播放时间

    //pose change about
    std::unordered_map<std::string, dx::XMMATRIX> PerviousPose;
    std::unordered_map<std::string, dx::XMMATRIX> TargetPose;
    float currentTime;
    float TransitionTime;
    bool OnTransition = false;
};


class AnimInstance {
    friend class UAnimationComponent;
public:
    AnimInstance(SkeletonMesh& mesh);
    virtual void Update(float deltaTime);
    void SetCurrentAnim(AnimAsset Anim);
    void SetAnimStateMachine(std::shared_ptr<AnimStateMachine> AnimStateMachine);
    void BlendAnimations(const AnimAsset& animA, const AnimAsset& animB, float blendFactor);
protected:
    SkeletonMesh& mesh;
    AnimAsset currentAnim;
    std::unordered_map<std::string, dx::XMMATRIX> currentPose;
    bool loopPlay = false;
    std::shared_ptr<AnimStateMachine> StateMachine;
    float currentTime = 0.0f;
};