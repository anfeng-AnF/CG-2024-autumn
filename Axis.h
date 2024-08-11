#pragma once
#include "Drawable.h"


class Axis :public Drawable {
public:
	Axis(Graphics &gfx);

	// Í¨¹ý Drawable ¼Ì³Ð
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Update(float dt) noexcept override;
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override;
};