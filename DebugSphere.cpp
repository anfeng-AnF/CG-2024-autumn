#include "DebugSphere.h"
#include "BindableCommon.h"
#include "Sphere.h"

DebugSphere::DebugSphere(Graphics& gfx, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float r):
	pos(pos),
	color(color)
{
	using namespace Bind;

	auto model = Sphere::Make(r);


	static int idint = 0;
	id = "DebugSphere" + std::to_string(idint++);
	auto vbuf = VertexBuffer::Resolve(gfx, id, model.vertices);
	id = vbuf->GetUID();
	AddBind(vbuf);
	
	AddBind(IndexBuffer::Resolve(gfx, "DebugSphere", model.indices));

	AddBind(PixelShader::Resolve(gfx, "DebugSpherePS.cso"));

	auto pvs = VertexShader::Resolve(gfx, "DebugSphereVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}


DirectX::XMMATRIX DebugSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x,pos.y,pos.z);
}
