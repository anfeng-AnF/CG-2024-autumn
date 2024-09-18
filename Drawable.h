#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Draw(Graphics& gfx) const noexcept;

	virtual ~Drawable() = default;
	virtual void destory() {
		for (auto bind : binds) {
			bind.reset();
		}
	};
protected:
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
	void AddBind(std::shared_ptr<Bind::Bindable> bind);

	std::vector<std::shared_ptr<Bind::Bindable>> binds;
	const class Bind::IndexBuffer* pIndexBuffer = nullptr;
private:
};