#include <memory>
#include <DirectXMath.h>
#include "AActor.h"

class APawn : public AActor {
public:
    APawn();
    virtual ~APawn();

    // ÿ֡����
    virtual void Tick(float DeltaTime) override;

    // �����ƶ�����
    void SetMovementDirection(const DirectX::XMFLOAT3& NewDirection);

    // ִ����Ծ
    virtual void Jump();

    // ������Ծ״̬
    virtual void UpdateJump(float DeltaTime);

protected:
    // �ƶ��ٶ�
    float MoveSpeed;

    // ��Ծ����
    float JumpForce;

    // ��ǰ�ƶ�����
    DirectX::XMFLOAT3 MovementDirection;

    // ��Ծ״̬
    bool bIsJumping;

    // �������ٶ�
    float Gravity;

    // ��ǰ�ٶ�
    DirectX::XMFLOAT3 Velocity;
};
