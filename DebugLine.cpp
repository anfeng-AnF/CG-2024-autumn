#include "DebugLine.h"
#include "BindableCommon.h"
#include "Transform.h"

std::string XMFLOAT3ToString(const DirectX::XMFLOAT3& vec) {
	std::ostringstream oss;
	oss << vec.x << "_" << vec.y << "_" << vec.z;
	return oss.str();
}

DebugLine::DebugLine(Graphics& gfx, DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end,DirectX::XMFLOAT3 color)
{
	using Dvtx::VertexLayout;
	using namespace Bind;
	Dvtx::VertexBuffer vbf(Dvtx::VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Float3Color)
	);
	std::vector<uint16_t> indices = { 0,1 };
	vbf.EmplaceBack(begin, color);
	vbf.EmplaceBack(end, color);
	std::string idStr = "Line_" + XMFLOAT3ToString(begin) + "_" + XMFLOAT3ToString(end) + "_" + XMFLOAT3ToString(color);

	AddBind(VertexBuffer::Resolve(gfx, "Line"+idStr, vbf));

	AddBind(IndexBuffer::Resolve(gfx, "Line", indices));

	AddBind(PixelShader::Resolve(gfx, "AxisPS.cso"));

	auto pvs = VertexShader::Resolve(gfx, "AxisVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(InputLayout::Resolve(gfx, vbf.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D_PRIMITIVE_TOPOLOGY_LINELIST));
}

DebugLine::DebugLine(Graphics& gfx, DirectX::XMFLOAT3 rayOrigin, DirectX::XMFLOAT3 rayDirection, float length, DirectX::XMFLOAT3 color)
	:DebugLine(gfx,rayOrigin,rayOrigin+DirectX::XMFLOAT3(rayDirection*length),color)
{
}

DirectX::XMMATRIX DebugLine::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
}

DottedLine::DottedLine(Graphics& gfx, DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3 color)
{
	using Dvtx::VertexLayout;
	using namespace Bind;
	Dvtx::VertexBuffer vbf(Dvtx::VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Float3Color)
	);
	std::vector<uint16_t> indices = { 0,1 };
	vbf.EmplaceBack(begin, color);
	vbf.EmplaceBack(end, color);
	static int id = 0;
	AddBind(VertexBuffer::Resolve(gfx, "dotLine" + std::to_string(id++), vbf));

	AddBind(IndexBuffer::Resolve(gfx, "dotLine", indices));

	AddBind(GeometryShader::Resolve(gfx, "DottedLineGS.cso"));

	AddBind(PixelShader::Resolve(gfx, "AxisPS.cso"));

	auto pvs = VertexShader::Resolve(gfx, "AxisVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(InputLayout::Resolve(gfx, vbf.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D_PRIMITIVE_TOPOLOGY_LINELIST));
}

DirectX::XMMATRIX DottedLine::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
}
