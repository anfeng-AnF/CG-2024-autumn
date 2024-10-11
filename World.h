#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include "UCollisionComponent.h"
class AActor; // 前向声明 AActor 类

struct FCollisionResult
{
    FHitResult HitResult;
    std::shared_ptr<AActor>HitActor;
};

class UWorld {
public:
    UWorld() {};
    ~UWorld() {};

    void AddActor(std::shared_ptr<AActor> Actor);  // 添加 Actor
    void RemoveActor(std::shared_ptr<AActor> Actor); // 移除 Actor
    void Tick(float DeltaTime); // 每帧更新所有 Actor
    void Render(); // 渲染所有 Actor
    std::vector<FCollisionResult> QuaryCollision(std::shared_ptr<AActor> Actor);
private:
    std::unordered_set<std::shared_ptr<AActor>> ActiveGroupActors; // 存储活跃的 Actor

    //// 可能的场景信息
    //float WorldWidth;   // 场景宽度
    //float WorldHeight;  // 场景高度
    //float WorldDepth;   // 场景深度
};