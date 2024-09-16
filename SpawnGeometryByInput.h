#pragma once
#include "CollisionGeometry.h"
#include "CtrlableGeometry.h"
#include "InputStateMachine.h"
#include "Vertex.h"
std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> CreateLineWithAdjacency(
	const XMFLOAT3& point1, const XMFLOAT3& point2);
std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> CreateCircleWithAdjacency(
	const XMFLOAT3& center, const XMFLOAT3& edgePoint, const XMFLOAT3& normal, unsigned int segmentCount = 20);
class SpawnGeometryByInput
{
	friend class SpawnGeoInputState;
public:
	class SpawnGeoInputState :public InputState
	{
	public:
		SpawnGeoInputState(Window& wnd,SpawnGeometryByInput* pSpawnGeo);
		// 通过 InputState 继承
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;

	private:
		SpawnGeometryByInput* pSpawnGeo;
		ImVec2 wndPos;
		bool choosedMehod = false;
		bool onDrawing = false;
		bool drawingEnd = false;
	}inputState;
	enum SpawnGeoMehod
	{
		NONE,
		LINE,					//一次一条
		LINE_CONTINUE,			//画完一条直线继续
		CIRCLE,					//圆心，半径
		CIRCLE_ARC,				//圆弧起点，终点，圆心
	};
public:
	SpawnGeometryByInput(Window&wnd,Camera&cam ,CollisionGeoManager*CGM);

	bool SpawnLine(screenPos pos,bool lpressed,SpawnGeoMehod SGmehod);//line with circle
	//bool SpawnLineContinue(screenPos pos, bool endFlag=false);
	bool SpawnCircleArc(screenPos pos,bool lpresed);

	bool SpawnImGuiWnd(ImVec2 windowPos);

	void draw(Graphics& gfx);
private:

	XMFLOAT3 GetIntersectionPlaneLine(LineRay plane, LineRay line);//Plane point formula and linear pointwise formula


private:
	//basic valuables
	CollisionGeoManager* CGM;
	Window& wnd;
	Camera& cam;

	SpawnGeoMehod mehod;

	std::shared_ptr<CollisionGeometry> drawingGeo;
};


