#pragma once

#include "IBindable.h"
#include <DirectXMath.h>
#include <vector>
#include <memory>

class Drawable {
public:
	Drawable();

	void addBindable(std::shared_ptr<IBindable> bindable);

	void draw(Graphics& gfx, DirectX::FXMMATRIX transform) const;

private:
	std::vector<std::shared_ptr<IBindable>> bindables;

	size_t indexCount;
	size_t instanceCount;
};
