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
	return false;
}
