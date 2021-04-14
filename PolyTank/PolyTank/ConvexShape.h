#pragma once

#include "Util.h"

class ConvexShape {
public:
	virtual ~ConvexShape() = default;
	virtual DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const = 0;
	virtual DirectX::XMMATRIX inertiaTensor() const = 0;

	bool checkIntersection(
		ConvexShape* pOther,
		DirectX::FXMMATRIX thisTransform,
		DirectX::FXMMATRIX otherTransform,
		DirectX::XMVECTOR* pResolution);

};

class Box : public ConvexShape {
public:
	Box(float width, float height, float depth);
	
	DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const override;
	DirectX::XMMATRIX inertiaTensor() const override;

private:
	float w, h, d;

};
