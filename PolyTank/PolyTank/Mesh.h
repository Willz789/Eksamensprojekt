#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include <vector>

class Mesh
{
public:
	void draw(Graphics& gfx, DirectX::XMMATRIX transform);
	void addDrawable(Drawable&& drawable);

private:
	std::vector<Drawable> drawables;

};
