#pragma once
#include "CollisionGeometry.h"
#include "CtrlableGeometry.h"
#include "InputStateMachine.h"
#include "Vertex.h"

class SpawnGeometryByInput
{
	friend class SpawnGeoInputState;
public:
	class SpawnGeoInputState :public InputState
	{
	public:
		SpawnGeoInputState();
		// 通过 InputState 继承
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;

	private:
		SpawnGeometryByInput* pSpawnGeo;
	};
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
	bool SpawnLineContinue(screenPos pos, bool endFlag=false);
	bool SpawnCircleArc(screenPos pos,bool lpresed);

	bool SpawnImGuiWnd();

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


