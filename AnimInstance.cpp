#include "AnimInstance.h"

AnimInstance::AnimInstance(SkeletonMesh& mesh)
	:
	mesh(mesh)
{
}

void AnimInstance::Update(float deltaTime)
{
	currentTime += deltaTime;
	currentTime =std::fmod(currentTime,currentAnim.GetDuration());
	OutputDebugStringA(std::to_string(currentTime).c_str());
	OutputDebugStringA("\n");
	auto curentPose = currentAnim.GetTransformBoneName_tm(currentTime);
	mesh.SetBonesTransform(curentPose);
}

