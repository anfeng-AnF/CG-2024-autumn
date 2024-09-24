#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "SkeletonMesh.h"
namespace dx = DirectX;


struct BoneAnimInfo
{
    std::string boneName;
    std::map<float, dx::XMVECTOR> translation;
    std::map<float, dx::XMVECTOR> rotation;
    std::map<float, dx::XMVECTOR> scale;
};

//Each class stores an animation
class AnimAsset {
public:
    static std::vector<AnimAsset> ReadAnimAssertFromFile(std::string fileName);
    std::unordered_map<std::string, dx::XMMATRIX> GetTransformBoneName_tm(float time);
    float GetDuration();

private:
    dx::XMMATRIX InterpolateTranslation(int boneIdx, float time);
    dx::XMMATRIX InterpolateScale(int boneIdx, float time);
    dx::XMMATRIX InterpolateRotation(int boneIdx, float time);


private:
    std::string name;
    std::vector<BoneAnimInfo> anim;
    float tickPerSecond;
    float duration;         //Total duration of animation
};

