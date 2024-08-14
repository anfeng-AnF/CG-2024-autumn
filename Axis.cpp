#include "Axis.h"
#include "Vertex.h"
#include "BindableCommon.h"


namespace dx = DirectX;

void GetAxis(Dvtx::VertexBuffer& vbf, std::vector<uint16_t>& indices) {
	const float AxisMax = 20.0f;
	const float AxisMinWidth = 1.0f;

	float arrowSize = 0.1f;
	float arrowLength = 0.2f;

	uint16_t index = 0;
	for (float x = -AxisMax; x <= AxisMax; x += AxisMinWidth) {
		for (float y = -AxisMax; y <= AxisMax; y += AxisMinWidth) {
			if (x == 0 || y == 0)continue;
			dx::XMFLOAT3 color;
			if (x != 0 && y != 0) {
				color = { 0.3f,0.3f,0.3f };
			}
			else
			{
				color = { 0.0f, 0.0f, 0.0f };
			}
			vbf.EmplaceBack(
				dx::XMFLOAT3{ x,0,y },
				color);
			if (y != -AxisMax) {
				indices.push_back(index);
				indices.push_back(index - 1);
			}
			if (x != -AxisMax) {
				indices.push_back(index);
				indices.push_back(index - 2 * AxisMax);
			}
			index++;
		}
	}
	// X 轴从 -AxisMax 到 AxisMax
	uint16_t vNum = vbf.Size();
	vbf.EmplaceBack(
		dx::XMFLOAT3{ -AxisMax, 0.0f, 0.0f },
		dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f } // 红色
	);
	vbf.EmplaceBack(
		dx::XMFLOAT3{ AxisMax, 0.0f, 0.0f },
		dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f } // 红色
	);
	indices.push_back(vNum);
	indices.push_back(vNum + 1);

	// 箭头的两个顶点
	vbf.EmplaceBack(
		dx::XMFLOAT3{ AxisMax - arrowLength, arrowSize, 0.0f },
		dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f } // 红色
	);
	vbf.EmplaceBack(
		dx::XMFLOAT3{ AxisMax - arrowLength, -arrowSize, 0.0f },
		dx::XMFLOAT3{ 1.0f, 0.0f, 0.0f } // 红色
	);

	// 添加 X 轴箭头的线段索引
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 2);
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 3);

	// Y 轴从 -AxisMax 到 AxisMax
	vNum = vbf.Size();
	vbf.EmplaceBack(
		dx::XMFLOAT3{ 0.0f, -AxisMax, 0.0f },
		dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f } // 绿色
	);
	vbf.EmplaceBack(
		dx::XMFLOAT3{ 0.0f, AxisMax, 0.0f },
		dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f } // 绿色
	);
	indices.push_back(vNum);
	indices.push_back(vNum + 1);

	// 箭头的两个顶点
	vbf.EmplaceBack(
		dx::XMFLOAT3{ arrowSize, AxisMax - arrowLength, 0.0f },
		dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f } // 绿色
	);
	vbf.EmplaceBack(
		dx::XMFLOAT3{ -arrowSize, AxisMax - arrowLength, 0.0f },
		dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f } // 绿色
	);

	// 添加 Y 轴箭头的线段索引
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 2);
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 3);


	//Z
	vNum = vbf.Size();
	vbf.EmplaceBack(
		dx::XMFLOAT3{ 0.0f,0.0f,-AxisMax },
		dx::XMFLOAT3{ 0.0f,0.0f,1.0f });
	vbf.EmplaceBack(
		dx::XMFLOAT3{ 0.0f,0.0f,AxisMax },
		dx::XMFLOAT3{ 0.0f,0.0f,1.0f }
	);
	indices.push_back(vNum);
	indices.push_back(vNum + 1);

	// 箭头的两个顶点
	vbf.EmplaceBack(
		dx::XMFLOAT3{ arrowSize, 0.0f, AxisMax - arrowLength },
		dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f } // 蓝色
	);
	vbf.EmplaceBack(
		dx::XMFLOAT3{ -arrowSize, 0.0f, AxisMax - arrowLength },
		dx::XMFLOAT3{ 0.0f, 0.0f, 1.0f } // 蓝色
	);

	// 添加 Z 轴箭头的线段索引
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 2);
	indices.push_back(vNum + 1);
	indices.push_back(vNum + 3);
}

Axis::Axis(Graphics&gfx)
{
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbf(Dvtx::VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Float3Color)
	);
	std::vector<uint16_t> indices;

	GetAxis(vbf, indices);


	AddBind(std::make_unique<VertexBuffer>(gfx, vbf));
	
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

	AddBind(std::make_unique<PixelShader>(gfx, L"AxisPS.cso"));

	auto pvs = std::make_unique<VertexShader>(gfx, L"AxisVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<InputLayout>(gfx, vbf.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_LINELIST));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX Axis::GetTransformXM() const noexcept
{
	return DirectX::XMMATRIX(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
}

void Axis::Update(float dt) noexcept
{
}

const std::vector<std::unique_ptr<Bindable>>& Axis::GetStaticBinds() const noexcept
{
	return std::vector<std::unique_ptr<Bindable>>{};
}
