#include "AnimInstance.h"
#include "Transform.h"
#include <stdexcept>
#include <DirectXMath.h>

using namespace DirectX;

AnimInstance::AnimInstance(SkeletonMesh& mesh)
	:
	mesh(mesh)
{
}

void AnimInstance::Update(float deltaTime)
{
	currentTime += deltaTime;
	currentTime =std::fmod(currentTime,currentAnim.GetDuration()/currentAnim.GetTickPerSecond());
	//OutputDebugStringA(std::to_string(currentTime).c_str());
	//OutputDebugStringA("\n");
	currentPose =currentAnim.GetTransformBoneName_tm(currentTime);
	mesh.SetBonesTransform(currentPose);
}

void AnimInstance::SetCurrentAnim(AnimAsset Anim)
{
	currentAnim = Anim;
}

void AnimInstance::SetAnimStateMachine(std::shared_ptr<AnimStateMachine> AnimStateMachine)
{
    StateMachine = AnimStateMachine;
}

void AnimInstance::BlendAnimations(const AnimAsset& animA, const AnimAsset& animB, float blendFactor)
{
}



void AnimStateMachine::AddState(const std::string& stateName, std::shared_ptr<AnimAsset> animation, float DefaultPlayRate)
{
    if (this->states.find(stateName) == this->states.end())
    {
        this->states[stateName] = State{ animation,DefaultPlayRate,{} };
    }
    else
    {
        throw std::runtime_error("State " + stateName + " already exists in the state machine.");
    }
}

void AnimStateMachine::SetTransitionCondition(const std::string& fromState, const std::string& toState, std::function<bool()> condition, float TransitionTime)
{
    if (this->states.find(fromState) == this->states.end())
    {
        throw std::runtime_error("State " + fromState + " does not exist in the state machine.");
    }

    if (this->states.find(toState) == this->states.end())
    {
        throw std::runtime_error("State " + toState + " does not exist in the state machine.");
    }

    this->states[fromState].transitions[toState] = { condition,TransitionTime };
}


void AnimStateMachine::Update(float deltaTime) {
    if (OnTransition) {
        UpdateTransition(deltaTime);
    }
    else
    {
        float time = fmod(AnimTime * states[currentState].playRate, states[currentState].animation->GetDuration()/ states[currentState].animation->GetTickPerSecond());
        CurrentPose = states[currentState].animation->GetTransformBoneName_tm(time);
        AnimTime+=deltaTime;

        for (const auto& condition : states[currentState].transitions)
        {
            if (condition.second.first()) {
                TransitionTime = condition.second.second;
                OnTransition = true;
                PerviousPose = CurrentPose;
                currentState = condition.first;
                TargetPose = states[currentState].animation->GetTransformBoneName_tm(0.0f);
                currentTime = 0.0f;
                AnimTime = 0.0f;
                break;
            }
        }
    }
}


std::unordered_map<std::string, dx::XMMATRIX> AnimStateMachine::GetCurrentPose()
{
	return CurrentPose;
}

void AnimStateMachine::SetCurrentState(std::string curState)
{
    if (this->states.find(curState) == this->states.end())
    {
        throw std::runtime_error("State " + curState + " does not exist in the state machine.");
    }
    else
    {
        currentState = curState;
        currentTime = 0.0f;
        AnimTime = 0.0f;
    }
}

void AnimStateMachine::UpdateTransition(float deltaTime)
{
    currentTime += deltaTime;
    float t = 0.0f;
    if (TransitionTime > 0.0f)
    {
        t = min(currentTime / TransitionTime, 1.0f);
        for (const auto& [boneName, previousPoseMatrix] : PerviousPose) {
            auto targetPoseMatrix = TargetPose[boneName];
            auto interpolatedPoseMatrix = XMMatrixLerp(previousPoseMatrix, targetPoseMatrix, t);
            CurrentPose[boneName] = interpolatedPoseMatrix;
        }
    }
    else
    {
        CurrentPose = TargetPose;
        OnTransition = false;
    }

    if (t >= 1.0f) {
        OnTransition = false;
        currentTime = 0.0f;
        TransitionTime = 0.0f;
    }
}

