#include "Mesh.h"

void Mesh::draw(Graphics& gfx, DirectX::XMMATRIX transform)
{
	for (Drawable& d : drawables) {
		d.draw(gfx, transform);
	}
}

void Mesh::addDrawable(Drawable&& drawable)
{
	drawables.emplace_back(std::move(drawable));
}

void Mesh::reset()
{
	drawables.clear();
}
