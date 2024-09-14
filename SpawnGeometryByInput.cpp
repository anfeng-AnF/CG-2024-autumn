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
}

void SpawnGeometryByInput::SpawnGeoInputState::Update(float deltaTime)
{
	//should exit state?
	//spawn ctrl wnd
	if (!choosedMehod) {
		choosedMehod = pSpawnGeo->SpawnImGuiWnd(wndPos);
	}

	if (choosedMehod
		&& pSpawnGeo->mehod == NONE 
		|| wnd.Kbd.KeyIsPressed(VK_ESCAPE)) {
		this->Machine->SetState(PERVIOUS_STATE);
	}bool methodIsNone = (pSpawnGeo->mehod == NONE);

	//Handle keyboard input

	//Handling of mouse input
	while (const auto delta = wnd.mouse.Read()) {
		switch (delta->GetType())
		{
		case Mouse::Event::Type::RPress:
			this->Machine->SetState("CameraMove");
			break;
		case Mouse::Event::Type::LPress:
			onDrawing = true;
			break;


		default:
			break;
		}

	}


}

void SpawnGeometryByInput::SpawnGeoInputState::Exit()
{
}


std::pair<Dvtx::VertexBuffer, std::vector<unsigned int>> CreateLineWithAdjacency(
	const XMFLOAT3& point1, const XMFLOAT3& point2)
{
	// 创建仅包含Position3D的顶点布局
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);


	// 添加四个点，依次为：邻接点1, 起点, 终点, 邻接点2
	vbuf.EmplaceBack(point1);  // 起点
	vbuf.EmplaceBack(point2);  // 终点

	// 构建带有邻接信息的线段的indices
	std::vector<unsigned int> indices = {
		0, 0, 1, 1  // adj1, point1, point2, adj2
	};

	return { std::move(vbuf), indices };
}
std::pair<Dvtx::VertexBuffer, std::vector<unsigned int>> CreateCircleWithAdjacency(
	const XMFLOAT3& center, const XMFLOAT3& edgePoint, unsigned int segmentCount = 20)
{
	// 创建仅包含Position3D的顶点布局
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);

	// 计算半径和圆上的起始点到圆心的向量
	XMFLOAT3 radiusVec{
		edgePoint.x - center.x,
		edgePoint.y - center.y,
		edgePoint.z - center.z
	};

	float radius = sqrtf(radiusVec.x * radiusVec.x + radiusVec.y * radiusVec.y + radiusVec.z * radiusVec.z);

	// 创建圆上的顶点
	float angleStep = XM_2PI / segmentCount;  // 每段的角度步进

	// 添加圆上的所有点
	for (unsigned int i = 0; i < segmentCount; ++i)
	{
		float angle = i * angleStep;
		float x = center.x + radius * cosf(angle);
		float y = center.y;  // 保持与圆心同一个平面
		float z = center.z + radius * sinf(angle);

		vbuf.EmplaceBack(XMFLOAT3{ x, y, z });
	}

	// 构建邻接信息的indices
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < segmentCount; ++i)
	{
		unsigned int nextIndex = (i + 1) % segmentCount;
		indices.push_back(i);         // 当前顶点
		indices.push_back(i);         // 当前顶点邻接点（自己）
		indices.push_back(nextIndex); // 下一个顶点
		indices.push_back(nextIndex); // 下一个顶点邻接点（自己）
	}

	return { std::move(vbuf), indices };
}
std::pair<Dvtx::VertexBuffer, std::vector<unsigned int>> CreateArcWithAdjacency(
	const XMFLOAT3& arcStart, const XMFLOAT3& arcEnd, const XMFLOAT3& center, unsigned int segmentCount = 20)
{
	// 创建仅包含Position3D的顶点布局
	Dvtx::VertexBuffer vbuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
	);

	// 将输入点转为 XMVECTOR 以便于计算
	using namespace DirectX;
	XMVECTOR centerVec = XMLoadFloat3(&center);
	XMVECTOR startVec = XMLoadFloat3(&arcStart);
	XMVECTOR endVec = XMLoadFloat3(&arcEnd);

	// 计算起点和终点的方向向量
	XMVECTOR radiusStartVec = XMVector3Normalize(XMVectorSubtract(startVec, centerVec));
	XMVECTOR radiusEndVec = XMVector3Normalize(XMVectorSubtract(endVec, centerVec));

	// 计算起点与终点间的夹角（弧度）
	float angle = XMVectorGetX(XMVector3AngleBetweenVectors(radiusStartVec, radiusEndVec));

	// 判断起点和终点的方向是否一致，以确定顺时针还是逆时针
	XMVECTOR crossProduct = XMVector3Cross(radiusStartVec, radiusEndVec);
	if (XMVectorGetY(crossProduct) < 0)
	{
		angle = XM_2PI - angle; // 如果叉积为负，说明是顺时针
	}

	// 计算每一段的角度步进
	float angleStep = angle / segmentCount;

	// 添加圆弧上的顶点
	for (unsigned int i = 0; i <= segmentCount; ++i)
	{
		float currentAngle = i * angleStep;
		// 通过旋转起始向量来生成弧上的点
		XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), currentAngle);
		XMVECTOR currentPoint = XMVector3Transform(radiusStartVec, rotationMatrix);
		currentPoint = XMVectorScale(currentPoint, XMVectorGetX(XMVector3Length(XMVectorSubtract(startVec, centerVec))));
		currentPoint = XMVectorAdd(currentPoint, centerVec);

		// 存入顶点缓冲区
		XMFLOAT3 vertex;
		XMStoreFloat3(&vertex, currentPoint);
		vbuf.EmplaceBack(vertex);
	}

	// 构建邻接信息的indices
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < segmentCount; ++i)
	{
		indices.push_back(i);         // 当前顶点
		indices.push_back(i);         // 当前顶点邻接点（自己）
		indices.push_back(i + 1);     // 下一个顶点
		indices.push_back(i + 1);     // 下一个顶点邻接点（自己）
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
	static bool called = false;
	LineRay line(pos, wnd, cam);
	static LineRay plane;
	static XMFLOAT3 perPoint;
	if (!called) {
		perPoint = GetIntersectionPlaneLine(plane, line);
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
	else
	{
		auto point = GetIntersectionPlaneLine(plane, line);
		Dvtx::VertexBuffer vbuf(
			Dvtx::VertexLayout{}
			.Append(Dvtx::VertexLayout::Position3D)
		);
		std::vector<unsigned int> indices;
		switch (SGmehod)
		{
		case SpawnGeometryByInput::LINE:
			std::tie(vbuf, indices) = CreateLineWithAdjacency(perPoint, point);
			break;
		case SpawnGeometryByInput::CIRCLE:
			std::tie(vbuf, indices) = CreateCircleWithAdjacency(perPoint, point);
			break;
		default:
			break;
		}
		//drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.1f);
	}

	if (lpressed) 
	{
		CGM->AddGeometry(drawingGeo);
		drawingGeo = nullptr;
		called = false;
		return true;
	}
	called = true;
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
		perPoint.push_back(GetIntersectionPlaneLine(plane, line));
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
	
	if (lpressed) 
	{
		perPoint.push_back(curPoint);
		num++;
	}

	if (num == 1) {
		auto [vbuf, indices] = CreateArcWithAdjacency (perPoint[0], perPoint[1], curPoint);
		//drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.1f);
	}




	if (num == 2) {
		CGM->AddGeometry(drawingGeo);
		drawingGeo = nullptr;
		num = 0;
		return true;
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
	selected |= ImGui::RadioButton("multi line", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::LINE_CONTINUE));
	selected |= ImGui::RadioButton("circle", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE));
	selected |= ImGui::RadioButton("circle arc", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE_ARC));
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
