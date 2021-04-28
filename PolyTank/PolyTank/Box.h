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

class TransformedBox : public TransformedShape {
public:
	TransformedBox(const Box& box, DirectX::FXMMATRIX transform);

	DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const override;
	AABB getBoundingBox() const override;

private:
	DirectX::XMFLOAT3 corners[8];
};
