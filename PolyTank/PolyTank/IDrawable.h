#pragma once

#include "IBindable.h"
#include <DirectXMath.h>
#include <vector>
#include <memory>

class IDrawable {
public:	
	virtual ~IDrawable() = default; // IDrawable can be held by unique_ptrs

	virtual void draw(Graphics& gfx, DirectX::FXMMATRIX transform) const = 0;
};
