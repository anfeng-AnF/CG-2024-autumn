#pragma once
#include "AnimInstance.h"
#include "UActorComponent.h"

class UAnimationComponent : public UActorComponent {
    using UAnimation = AnimInstance;
public:
    UAnimationComponent();
    ~UAnimationComponent();

    // ���õ�ǰ����
    void SetAnimation(std::shared_ptr<UAnimation> Anim);

    // ���¶���
    void Update(float DeltaTime);

    // ���Ŷ���
    void Play();

    // ֹͣ����
    void Stop();

    // ��鶯���Ƿ����ڲ���
    bool IsPlaying() const;

private:
    std::shared_ptr<UAnimation> CurrentAnimation; // ��ǰ����
    float PlaybackSpeed; // �����ٶ�
    float CurrentTime; // ��ǰʱ��
    bool bIsPlaying; // �Ƿ����ڲ���
};