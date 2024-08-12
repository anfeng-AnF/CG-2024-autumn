#pragma once
#include "Drawable.h"

using namespace Bind;
class Axis :public Drawable {
public:
	Axis(Graphics &gfx);

	// ͨ�� Drawable �̳�
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Update(float dt) noexcept override;
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override;
};