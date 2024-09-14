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
		// ͨ�� InputState �̳�
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;

	private:
		SpawnGeometryByInput* pSpawnGeo;
	};
	enum SpawnGeoMehod
	{
		NONE,
		LINE,					//һ��һ��
		LINE_CONTINUE,			//����һ��ֱ�߼���
		CIRCLE,					//Բ�ģ��뾶
		CIRCLE_ARC,				//Բ����㣬�յ㣬Բ��
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


