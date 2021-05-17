#pragma once

#include "Util.h"
#include "ConvexShape.h"

class Box : public ConvexShape {
	friend class TransformedBox;
public:
	Box(float width, float height, float depth);

	std::unique_ptr<TransformedShape> transform(DirectX::FXMMATRIX m) const override;
	DirectX::XMMATRIX inertiaTensor() const override;

private:
	float w, h, d;
};
