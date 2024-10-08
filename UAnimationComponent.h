#pragma once
#include "AnimInstance.h"
#include "UActorComponent.h"

class UAnimationComponent : public UActorComponent {
    using UAnimation = AnimInstance;
public:
    UAnimationComponent();
    ~UAnimationComponent();

    // 设置当前动画
    void SetAnimation(std::shared_ptr<UAnimation> Anim);

    // 更新动画
    void Update(float DeltaTime);

    // 播放动画
    void Play();

    // 停止动画
    void Stop();

    // 检查动画是否正在播放
    bool IsPlaying() const;

private:
    std::shared_ptr<UAnimation> CurrentAnimation; // 当前动画
    float PlaybackSpeed; // 播放速度
    float CurrentTime; // 当前时间
    bool bIsPlaying; // 是否正在播放
};