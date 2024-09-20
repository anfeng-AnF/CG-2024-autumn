#include "SpawnGeometryByInput.h"

SpawnGeometryByInput::SpawnGeoInputState::SpawnGeoInputState(Window& wnd,SpawnGeometryByInput* pSpawnGeo)
	:
	InputState(wnd),
	pSpawnGeo(pSpawnGeo)
{
}

void SpawnGeometryByInput::SpawnGeoInputState::Enter()
{
	wnd.mouse.DisableRaw();
	wnd.EnableCursor();
	wndPos = ImVec2{ (float)wnd.mouse.GetPosX(),(float)wnd.mouse.GetPosY() };
	choosedMehod = false;
	onDrawing = false;
	drawingEnd = false;
}

void SpawnGeometryByInput::SpawnGeoInputState::Update(float deltaTime)
{
	pSpawnGeo->draw(wnd.Gfx());

	//should exit state?
	//spawn ctrl wnd
	if (!choosedMehod) {
		choosedMehod = pSpawnGeo->SpawnImGuiWnd(wndPos);
	}

	if (choosedMehod && pSpawnGeo->mehod == NONE
		|| wnd.Kbd.KeyIsPressed(VK_ESCAPE)
		|| drawingEnd)
	{
		this->Machine->SetState(PERVIOUS_STATE);
	}bool methodIsNone = (pSpawnGeo->mehod == NONE);

	//Handle keyboard input

	//Handling of mouse input
	while (const auto delta = wnd.mouse.Read()) {
		switch (delta->GetType())
		{
		case Mouse::Event::Type::RPress:
			if (!onDrawing) {
				this->Machine->SetState("CameraMove");
			}
			break;
		case Mouse::Event::Type::LPress:
			switch (pSpawnGeo->mehod)
			{
			case SpawnGeometryByInput::LINE:
			case SpawnGeometryByInput::CIRCLE:
				drawingEnd = pSpawnGeo->SpawnLine(delta.value().GetPos(), true, pSpawnGeo->mehod);
				break;
			case SpawnGeometryByInput::LINE_CONTINUE:
				break;
			case SpawnGeometryByInput::CIRCLE_ARC:
				drawingEnd = pSpawnGeo->SpawnCircleArc(delta.value().GetPos(), true);
				break;
			}
			onDrawing = true;
			break;
		default:

			break;
		}
		//handle spawn msg	
		if (onDrawing) {
			switch (pSpawnGeo->mehod)
			{
			case SpawnGeometryByInput::LINE:
			case SpawnGeometryByInput::CIRCLE:
				pSpawnGeo->SpawnLine(delta.value().GetPos(), false, pSpawnGeo->mehod);
				break;
			case SpawnGeometryByInput::LINE_CONTINUE:
				break;
			case SpawnGeometryByInput::CIRCLE_ARC:
				pSpawnGeo->SpawnCircleArc(delta.value().GetPos(), false);
				break;
			}
		}
	}
}

void SpawnGeometryByInput::SpawnGeoInputState::Exit()
{
}


std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> CreateLineWithAdjacency(
	const XMFLOAT3& point1, const XMFLOAT3& point2)
{

	// ����������Position3D�Ķ��㲼��
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);


	// ����ĸ��㣬����Ϊ���ڽӵ�1, ���, �յ�, �ڽӵ�2
	vbuf.EmplaceBack(point1);  // ���
	vbuf.EmplaceBack(point2);  // �յ�
	// ���������ڽ���Ϣ���߶ε�indices
	std::vector<uint16_t> indices = {
		0, 0, 1, 1  // adj1, point1, point2, adj2
	};

	return { std::move(vbuf), indices };
}
std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> CreateCircleWithAdjacency(
	const XMFLOAT3& center, const XMFLOAT3& edgePoint, const XMFLOAT3& normal, unsigned int segmentCount)
{
	// ����������Position3D�Ķ��㲼��
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);

	// ����뾶��Բ�ϵ���ʼ�㵽Բ�ĵ�����
	XMFLOAT3 radiusVec{
		edgePoint.x - center.x,
		edgePoint.y - center.y,
		edgePoint.z - center.z
	};

	float radius = sqrtf(radiusVec.x * radiusVec.x + radiusVec.y * radiusVec.y + radiusVec.z * radiusVec.z);

	// ����Բ����ƽ��ķ��ߺͻ��ڴ�ƽ��ĵ�λ����
	XMVECTOR norm = XMLoadFloat3(&normal);
	XMVECTOR tanget = XMVector3Normalize(XMLoadFloat3(&radiusVec));
	XMVECTOR planeNormal = XMVector3Normalize(norm);
	XMVECTOR circleCenter = XMLoadFloat3(&center);
	XMVECTOR biTanget =XMVector3Normalize(XMVector3Cross(tanget, planeNormal));

	float angleStep = XM_2PI / segmentCount;


	for (unsigned int i = 0; i < segmentCount; ++i)
	{
		float angle = i * angleStep;

		XMVECTOR point = XMVectorAdd(circleCenter,
			XMVectorScale(
			XMVectorAdd(
				XMVectorScale(biTanget,cosf(angle)),
				XMVectorScale(tanget,sinf(angle))
			),radius)
		);

		XMFLOAT3 vertex;
		XMStoreFloat3(&vertex, point);

		vbuf.EmplaceBack(vertex);
	}

	// �����ڽ���Ϣ��indices
	std::vector<uint16_t> indices;

	for (unsigned int i = 0; i < segmentCount; ++i)
	{
		int prevIndex = (i - 1 + segmentCount) % segmentCount;
		int currIndex = i;
		int nextIndex = (i + 1) % segmentCount;
		int nextNextIndex = (i + 2) % segmentCount;

		// ��� 4 ��������ǰһ�����㣬��ǰ���㣬��һ�����㣬����һ������
		indices.push_back(prevIndex);
		indices.push_back(currIndex);
		indices.push_back(nextIndex);
		indices.push_back(nextNextIndex);
	}

	return { std::move(vbuf), indices };
}


std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> CreateArcWithAdjacency(
	const XMFLOAT3& point1, const XMFLOAT3& point2, const XMFLOAT3& point3, unsigned int segmentCount)
{
	XMFLOAT3 center;
	float radius;

	// ����Բ�ĺͰ뾶
	//CalculateCircleCenterAndRadius(point1, point2, point3, center, radius);

	XMVECTOR centerVec = XMLoadFloat3(&center);
	XMVECTOR vec1 = XMLoadFloat3(&point1);
	XMVECTOR vec2 = XMLoadFloat3(&point2);

	// ����������յ��ļнǣ����ȣ�
	XMVECTOR radiusVec1 = XMVector3Normalize(XMVectorSubtract(vec1, centerVec));
	XMVECTOR radiusVec2 = XMVector3Normalize(XMVectorSubtract(vec2, centerVec));
	float angle = XMVectorGetX(XMVector3AngleBetweenVectors(radiusVec1, radiusVec2));

	// ������ת��
	XMVECTOR rotationAxis = XMVector3Cross(radiusVec1, radiusVec2);
	rotationAxis = XMVector3Normalize(rotationAxis);

	// �ж��Ƿ�˳ʱ��
	if (XMVectorGetY(rotationAxis) < 0)
	{
		angle = XM_2PI - angle;
	}

	// ����ÿһ�εĽǶȲ���
	float angleStep = angle / segmentCount;

	// �������㲼�ֺͶ��㻺����
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);

	// ���Բ���ϵĶ���
	for (unsigned int i = 0; i <= segmentCount; ++i)
	{
		float currentAngle = i * angleStep;

		// ʹ����ת�������Բ���ϵĵ�
		XMVECTOR pointOnArc = XMVector3Transform(radiusVec1, XMMatrixRotationAxis(rotationAxis, currentAngle));
		pointOnArc = XMVectorAdd(pointOnArc, centerVec);

		// ���붥�㻺����
		XMFLOAT3 vertex;
		XMStoreFloat3(&vertex, pointOnArc);
		vbuf.EmplaceBack(vertex);
	}

	// �����ڽ���Ϣ��indices
	std::vector<uint16_t> indices;
	for (uint16_t i = 0; i < segmentCount; ++i)
	{
		// ����ڽ���Ϣ����������ǰ���㣬��ǰ�ڽӶ��㣨�Լ�������һ�����㣬��һ���ڽӶ��㣨�Լ���
		indices.push_back(i);         // ��ǰ����
		indices.push_back(i);         // ��ǰ�����ڽӵ㣨�Լ���
		indices.push_back(i + 1);     // ��һ������
		indices.push_back(i + 1);     // ��һ�������ڽӵ㣨�Լ���
	}

	return { std::move(vbuf), indices };
}



SpawnGeometryByInput::SpawnGeometryByInput(Window& wnd, Camera& cam, CollisionGeoManager* CGM)
	:
	wnd(wnd),
	cam(cam),
	CGM(CGM),
	inputState(wnd,this)
{
}

bool SpawnGeometryByInput::SpawnLine(screenPos pos,bool lpressed,SpawnGeoMehod SGmehod)
{
	static int called = 0;
	LineRay line(pos, wnd, cam);
	static LineRay plane;
	static XMFLOAT3 perPoint;
	XMFLOAT3 point;
	if (!called) {
		auto camTransform = cam.GetTransform();
		XMStoreFloat3(&plane.rayDirection, camTransform.GetForwardVector());
		if (camTransform == Camera::FrontView || camTransform == Camera::RightView || camTransform == Camera::TopView) {
			plane.rayOrigin = { 0.0f,0.0f,0.0f };
		}
		else
		{
			plane.rayOrigin = plane.rayDirection * 30.0f;
		}
		perPoint = GetIntersectionPlaneLine(plane, line);
	}
	else
	{
		point = GetIntersectionPlaneLine(plane, line);
		switch (SGmehod)
		{
		case SpawnGeometryByInput::LINE: {
			auto [vbuf, indices] = CreateLineWithAdjacency(perPoint, point);
			drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 1.0f);
		}
			break;
		case SpawnGeometryByInput::CIRCLE: {
			auto [vbuf, indices] = CreateCircleWithAdjacency(perPoint, point,plane.rayDirection);
			drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 1.0f);
		}
			break;
		default:
			break;
		}
	}
	DebugGraphsMannger::GetInstence().AddGeo(std::make_shared<DebugSphere>(wnd.Gfx(), XMFLOAT3{ 1.0f,0.0f,0.0f }, point), 0.0f);
	DebugGraphsMannger::GetInstence().AddGeo(std::make_shared<DebugSphere>(wnd.Gfx(), XMFLOAT3{ 1.0f,0.0f,0.0f }, perPoint), 0.0f);

	if (lpressed) 
	{
		called++;
		if (called == 2) {
			called = 0;
			CGM->AddGeometry(drawingGeo);
			drawingGeo = nullptr;
			return true;
		}
	}
	return false;
}

bool SpawnGeometryByInput::SpawnCircleArc(screenPos pos,bool lpressed)
{
	static int num;
	LineRay line(pos, wnd, cam);
	static LineRay plane;
	static std::vector<XMFLOAT3> perPoint;
	XMFLOAT3 curPoint;
	if (!num) {
		auto camTransform = cam.GetTransform();
		XMStoreFloat3(&plane.rayDirection, camTransform.GetForwardVector());
		if (camTransform == Camera::FrontView || camTransform == Camera::RightView || camTransform == Camera::TopView) {
			plane.rayOrigin = { 0.0f,0.0f,0.0f };
		}
		else
		{
			plane.rayOrigin = plane.rayDirection * 30.0f;
		}
	}

	curPoint = GetIntersectionPlaneLine(plane, line);
	

	if (num == 1) {
		auto [vbuf, indices] = CreateLineWithAdjacency(perPoint[0], curPoint);
		drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.01f);
	}

	if (num == 2) {
		if (perPoint[1].x == curPoint.x&& perPoint[1].y == curPoint.y&& perPoint[1].z == curPoint.z)return false;
		auto [vbuf, indices] = CreateArcWithAdjacency (perPoint[0], perPoint[1], curPoint);
		drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.1f);
	}



	if (lpressed) 
	{
		perPoint.push_back(curPoint);
		num++;
		if (num == 3) {
			CGM->AddGeometry(drawingGeo);
			drawingGeo = nullptr;
			num = 0;
			return true;
		}
	}
	return false;
}


bool SpawnGeometryByInput::SpawnImGuiWnd(ImVec2 windowPos)
{
	ImGui::SetNextWindowPos(windowPos);
	bool selected=false;
	ImGui::Begin("Spawn Geos");
	selected |= ImGui::RadioButton("exit", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::NONE));
	selected |= ImGui::RadioButton("line", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::LINE));
	//selected |= ImGui::RadioButton("multi line", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::LINE_CONTINUE));
	selected |= ImGui::RadioButton("circle", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE));
	//selected |= ImGui::RadioButton("circle arc", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE_ARC));
	ImGui::End();
	return selected;
}

void SpawnGeometryByInput::draw(Graphics& gfx)
{
	if (drawingGeo) {
		drawingGeo->Draw(gfx);
	}
}

XMFLOAT3 SpawnGeometryByInput::GetIntersectionPlaneLine(LineRay plane, LineRay line)
{
	XMVECTOR planePoint = XMLoadFloat3(&plane.rayOrigin);
	XMVECTOR planeNormal = XMLoadFloat3(&plane.rayDirection);
	XMVECTOR lineOrigin = XMLoadFloat3(&line.rayOrigin);
	XMVECTOR lineDirection = XMLoadFloat3(&line.rayDirection);

	XMVECTOR numerator = XMVector3Dot(XMVectorSubtract(planePoint, lineOrigin), planeNormal);
	XMVECTOR denominator = XMVector3Dot(lineDirection, planeNormal);

	if (XMVector3Equal(denominator, XMVectorZero()))
	{
		throw std::runtime_error("The line is parallel to the plane or lies within the plane.");
	}

	float t = numerator.m128_f32[0] / denominator.m128_f32[0];

	XMVECTOR intersection = XMVectorAdd(lineOrigin, XMVectorScale(lineDirection, t));

	XMFLOAT3 intersectionPoint;
	XMStoreFloat3(&intersectionPoint, intersection);

	return intersectionPoint;
}
