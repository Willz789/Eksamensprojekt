#pragma once

#include "IBindable.h"
#include <vector>
#include <memory>

class Drawable {
public:
	void addBindable(std::shared_ptr<IBindable> bindable);

	void draw(Graphics& gfx) const;

private:
	std::vector<std::shared_ptr<IBindable>> bindables;

	size_t indexBufferSize;
};
