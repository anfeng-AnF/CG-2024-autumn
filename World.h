#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>

class AActor; // ǰ������ AActor ��

class UWorld {
public:
    UWorld() {};
    ~UWorld() {};

    void AddActor(std::shared_ptr<AActor> Actor);  // ��� Actor
    void RemoveActor(std::shared_ptr<AActor> Actor); // �Ƴ� Actor
    void Tick(float DeltaTime); // ÿ֡�������� Actor
    void Render(); // ��Ⱦ���� Actor

private:
    std::unordered_set<std::shared_ptr<AActor>> ActiveGroupActors; // �洢��Ծ�� Actor

    //// ���ܵĳ�����Ϣ
    //float WorldWidth;   // �������
    //float WorldHeight;  // �����߶�
    //float WorldDepth;   // �������
};