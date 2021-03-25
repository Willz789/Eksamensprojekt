#pragma once

#include "IDrawable.h"

class ParticleSystem : public IDrawable {
public:
	void draw(Graphics& gfx, DirectX::FXMMATRIX transform) const override;

	void addBindable(std::shared_ptr<IBindable> bindable);

private:
	std::vector<std::shared_ptr<IBindable>> bindables;

	size_t particleCount;
};

