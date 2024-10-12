#include "AnimInstance.h"

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

