#include "AnimAssert.h"

dx::XMVECTOR ConvertToXMVECTOR(const aiVector3D& vec) {
    return dx::XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
}
dx::XMVECTOR ConvertToXMVECTOR(const aiQuaternion& quat) {
    return dx::XMVectorSet(quat.x, quat.y,quat.z, quat.w);
}
std::vector<AnimAsset> AnimAsset::ReadAnimAssertFromFile(std::string fileName)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName.c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(std::string("Error loading file: ") + importer.GetErrorString());
        return {};
    }

    std::vector<AnimAsset> AAsset(scene->mNumAnimations);

    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        const aiAnimation* animation = scene->mAnimations[i];

        AAsset[i].name = animation->mName.C_Str();
        AAsset[i].duration = animation->mDuration/ animation->mTicksPerSecond;
        AAsset[i].tickPerSecond = animation->mTicksPerSecond;
        AAsset[i].anim.resize(animation->mNumChannels);

        for (unsigned int j = 0; j < animation->mNumChannels; j++) {
            const aiNodeAnim* channel = animation->mChannels[j];
            AAsset[i].anim[j].boneName = channel->mNodeName.C_Str();

            // Position
            for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                AAsset[i].anim[j].translation[channel->mPositionKeys[k].mTime / animation->mTicksPerSecond] = ConvertToXMVECTOR(channel->mPositionKeys[k].mValue);
            }

            // Scale
            for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                AAsset[i].anim[j].scale[channel->mScalingKeys[k].mTime/ animation->mTicksPerSecond] = ConvertToXMVECTOR(channel->mScalingKeys[k].mValue);
            }

            // Rotation
            for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                AAsset[i].anim[j].rotation[channel->mRotationKeys[k].mTime/ animation->mTicksPerSecond] = ConvertToXMVECTOR(channel->mRotationKeys[k].mValue);
            }
        }
    }

    return AAsset;
}

std::unordered_map<std::string, dx::XMMATRIX> AnimAsset::GetTransformBoneName_tm(float time)
{
    std::unordered_map<std::string, dx::XMMATRIX> transforms;

    for (int i = 0; i < anim.size(); i++) {
        transforms[anim[i].boneName] =
            InterpolateScale(i, time) *
            InterpolateRotation(i, time) *
            InterpolateTranslation(i, time);
    }

    return transforms;
}

float AnimAsset::GetDuration()
{
    return duration;
}

dx::XMMATRIX AnimAsset::InterpolateTranslation(int boneIdx, float time)
{
    auto up = anim[boneIdx].translation.lower_bound(time);

    if (up == anim[boneIdx].translation.end()) {
        return dx::XMMatrixIdentity();
    }

    if (up == anim[boneIdx].translation.begin()) {
        return dx::XMMatrixTranslationFromVector(up->second);
    }

    auto prev = std::prev(up);

    float deltaTime = up->first - prev->first;
    float factor = (time - prev->first) / deltaTime;

    auto interpolatedTranslation = dx::XMVectorLerp(prev->second, up->second, factor);

    return dx::XMMatrixTranslationFromVector(interpolatedTranslation);
}

dx::XMMATRIX AnimAsset::InterpolateScale(int boneIdx, float time)
{
    auto up = anim[boneIdx].scale.lower_bound(time);

    if (up == anim[boneIdx].scale.end()) {
        return dx::XMMatrixIdentity();
    }

    if (up == anim[boneIdx].scale.begin()) {
        return dx::XMMatrixScalingFromVector(up->second);
    }

    auto prev = std::prev(up);
    float deltaTime = up->first - prev->first;
    float factor = (time - prev->first) / deltaTime;
    auto interpolatedScale = dx::XMVectorLerp(prev->second, up->second, factor);

    return dx::XMMatrixScalingFromVector(interpolatedScale);
}

dx::XMMATRIX AnimAsset::InterpolateRotation(int boneIdx, float time)
{
    auto up = anim[boneIdx].rotation.lower_bound(time);

    if (up == anim[boneIdx].rotation.end()) {
        return dx::XMMatrixIdentity();
    }

    if (up == anim[boneIdx].rotation.begin()) {
        return dx::XMMatrixRotationQuaternion(up->second);
    }

    auto prev = std::prev(up);
    float deltaTime = up->first - prev->first;
    float factor = (time - prev->first) / deltaTime;
    auto interpolatedRotation = dx::XMQuaternionSlerp(prev->second, up->second, factor);

    return dx::XMMatrixRotationQuaternion(interpolatedRotation);
}



