#pragma once
#include "UCollisionComponent.h"
#include <vector>

class UConvexComponent : public UCollisionComponent {
public:
    UConvexComponent();
    virtual ~UConvexComponent();

    // ����͹����εĶ���
    void SetConvexVertices(const std::vector<DirectX::XMFLOAT3>& Vertices);

    // ��ȡ͹����εļ�����
    const std::vector<DirectX::XMFLOAT3>& GetConvexVertices() const;

    // ʵ�ּ����ײ�ĺ���
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const override;
private:
    std::vector<DirectX::XMFLOAT3> ConvexVertices;
    std::vector<int> indeices;
};
