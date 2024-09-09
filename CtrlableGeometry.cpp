#include "CtrlableGeometry.h"

TransformComponentBase::TransformComponentBase()
{
}

void TransformComponentBase::draw(Graphics& gfx) const noexcept
{
}

void TransformComponentBase::SetTransform(FTransform transform)
{
}

bool TransformComponentBase::TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
	auto res = pTransformCtrlComponent->TraceByLineGetNearestMesh(lineBeginPos, lineVector);
	if (res.second) {
		if (res.second->GetName() == "X") tAxis = X;
		if (res.second->GetName() == "Y") tAxis = Y;
		if (res.second->GetName() == "Z") tAxis = Z;
		if (res.second->GetName() == "XY")tAxis = XY;
		if (res.second->GetName() == "XZ")tAxis = XZ;
		if (res.second->GetName() == "YZ")tAxis = YZ;
		if (res.second->GetName() =="XYZ")tAxis = XYZ;

	}
	return false;
}

XMMATRIX TranslateComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
	return XMMATRIX();
}

XMMATRIX RotationComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
	return XMMATRIX();
}

XMMATRIX ScaleComponent::GetDeltaTransform(screenPos from, screenPos to, Window& wnd)
{
	return XMMATRIX();
}
