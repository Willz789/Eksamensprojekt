#pragma once

#include "Graphics.h"
#include "IDrawable.h"
#include "IBindable.h"

#include <vector>
#include <memory>


class Mesh : public IDrawable
{
public:
	void draw(Graphics& gfx, DirectX::XMMATRIX transform) const override;
	
	void addBindable(std::shared_ptr<IBindable> pBindable);

private:
	std::vector<std::shared_ptr<IBindable>> bindables;
	
	size_t indexCount;
};
