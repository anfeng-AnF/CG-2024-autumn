#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include "AActor.h"
#include "UCollisionComponent.h"
#include "Graphics.h"
class AActor;

struct FCollisionResult
{
    FHitResult HitResult;
    std::shared_ptr<AActor> HitActor;
};

class UWorld {
public:
    UWorld() {};
    ~UWorld() {};

    void AddActor(std::shared_ptr<AActor> Actor);  // ��� Actor
    void RemoveActor(std::shared_ptr<AActor> Actor); // �Ƴ� Actor
    void Tick(float DeltaTime); // ÿ֡�������� Actor
    void Render(Graphics&Gfx); // ��Ⱦ���� Actor
    std::vector<FCollisionResult> QuaryCollision(AActor* Actor);
private:
    std::unordered_set<std::shared_ptr<AActor>> ActiveGroupActors; // �洢��Ծ�� Actor

    //// ���ܵĳ�����Ϣ
    //float WorldWidth;   // �������
    //float WorldHeight;  // �����߶�
    //float WorldDepth;   // �������
};