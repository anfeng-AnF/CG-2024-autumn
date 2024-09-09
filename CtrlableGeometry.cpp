#include "CtrlableGeometry.h"

TransformComponentBase::TransformComponentBase():
	DGM(DebugGraphsMannger::GetDGMRefference())
{
}

void TransformComponentBase::draw(Graphics& gfx) const noexcept
{
	pGfx = &gfx;
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
		DebugSphere ds(*pGfx, XMFLOAT3{ 0.7f,0.1f,0.1f }, res.first.pos);
		DGM.AddGeo(&ds);
		res.second->SetSelect(true);
		ctrlingMesh = res.second;
		return true;
	}
	else
	{
		tAxis = NONE_AXIS;
	}
	return false;
}

void TransformComponentBase::EndTransform()
{
	if (ctrlingMesh) {
		ctrlingMesh->SetSelect(false);
		ctrlingMesh = nullptr;
	}
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
