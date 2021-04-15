#pragma once

#include "Util.h"
#include "TransformedShape.h"

class ConvexShape {
public:
	virtual ~ConvexShape() = default;
	virtual std::unique_ptr<TransformedShape> transform(DirectX::FXMMATRIX m) const = 0;
	virtual DirectX::XMMATRIX inertiaTensor() const = 0;
};


