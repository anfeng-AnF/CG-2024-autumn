#include "SpawnGeometryByInput.h"

void SpawnGeometryByInput::SpawnGeoInputState::Enter()
{
}

void SpawnGeometryByInput::SpawnGeoInputState::Update(float deltaTime)
{
}

void SpawnGeometryByInput::SpawnGeoInputState::Exit()
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
		switch (SGmehod)
		{
		case SpawnGeometryByInput::LINE:
			break;
		case SpawnGeometryByInput::CIRCLE:
			break;
		default:
			break;
		}
		//auto [vbuf, indices] = ;
		Dvtx::VertexBuffer vbuf{ Dvtx::VertexLayout{} };
		std::vector<uint16_t> indices;
		drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.1f);
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
		auto [vbuf, indices] = func_circle_arc(perPoint[0], perPoint[1], curPoint);
		drawingGeo = std::make_shared<WidthLine>(wnd.Gfx(), cam, vbuf, indices, XMFLOAT3{ 0.0f,0.0f,0.0f }, 0.1f);
	}




	if (num == 2) {
		CGM->AddGeometry(drawingGeo);
		drawingGeo = nullptr;
		num = 0;
		return true;
	}
	return false;
}


bool SpawnGeometryByInput::SpawnImGuiWnd()
{
	bool selected=false;
	ImGui::BeginChild("Spawn Geos");
	selected |= ImGui::RadioButton("exit", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::NONE));
	selected |= ImGui::RadioButton("line", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::LINE));
	selected |= ImGui::RadioButton("multi line", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::LINE_CONTINUE));
	selected |= ImGui::RadioButton("circle", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE));
	selected |= ImGui::RadioButton("circle arc", reinterpret_cast<int*>(&mehod), static_cast<int>(SpawnGeoMehod::CIRCLE_ARC));
	ImGui::EndChild();
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
