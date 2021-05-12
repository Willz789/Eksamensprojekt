#pragma once

#include "Graphics.h"
#include "IDrawable.h"
#include "IBindable.h"

#include <vector>
#include <memory>

class Mesh : public IDrawable
{
public:
	Mesh() = default;

	void draw(Graphics& gfx, DirectX::XMMATRIX transform) const override;
	
	void addBindable(std::shared_ptr<IBindable> pBindable);
	template<typename T>
	std::shared_ptr<T> getBindableByType() const;

private:
	std::vector<std::shared_ptr<IBindable>> bindables;
	
	size_t indexCount;
};

template<typename T>
inline std::shared_ptr<T> Mesh::getBindableByType() const
{
	for (auto& pBind : bindables) {
		auto pT = std::dynamic_pointer_cast<T>(pBind);
		if (pT) {
			return pT;
		}
	}

	return nullptr;
}
