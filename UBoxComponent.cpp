#include "UBoxComponent.h"

UBoxComponent::UBoxComponent() {
    // Ĭ�ϳ�ʼ�����ӵı߽�
    Box = DirectX::BoundingBox();
}

UBoxComponent::~UBoxComponent() {}

void UBoxComponent::SetBoxExtent(const DirectX::XMFLOAT3& Extent) {
    Box.Extents = DirectX::XMFLOAT3(Extent.x, Extent.y, Extent.z);
    // ���º��ӵ�����λ�ã�����������ԭ��
    Box.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

const DirectX::BoundingBox& UBoxComponent::GetBoundingBox() const {
    return Box;
}

FHitResult UBoxComponent::CheckCollision(const UCollisionComponent& Other) const {
    FHitResult HitResult;

    // ������ݲ�ͬ����ײ���������Ӧ����ײ��ⷽ��
    if (const USphereComponent* Sphere = dynamic_cast<const USphereComponent*>(&Other)) {
        if (CheckCollisionWithSphere(*Sphere)) {
            // ��� HitResult
        }
    }
    else if (const UCapsuleComponent* Capsule = dynamic_cast<const UCapsuleComponent*>(&Other)) {
        if (CheckCollisionWithCapsule(*Capsule)) {
            // ��� HitResult
        }
    }
    else if (const UConvexComponent* Convex = dynamic_cast<const UConvexComponent*>(&Other)) {
        if (CheckCollisionWithConvex(*Convex)) {
            // ��� HitResult
        }
    }

    return HitResult; // ������ײ���
}

// ʾ������������ײ����ʵ��
bool UBoxComponent::CheckCollisionWithSphere(const USphereComponent& Sphere) const {
    return Box.Intersects(Sphere.GetBoundingSphere());
}

// ʾ�����뽺������ײ����ʵ��
bool UBoxComponent::CheckCollisionWithCapsule(const UCapsuleComponent& Capsule) const {
    // ʹ�ú����뽺�������ײ����㷨
    return Box.Intersects(Capsule.GetBoundingSphere());
}

// ʾ������͹�������ײ����ʵ��
bool UBoxComponent::CheckCollisionWithConvex(const UConvexComponent& Convex) const {
    // ʹ�ú�����͹����ε���ײ����㷨
    return Box.Intersects(Convex.GetConvexVertices());
}

