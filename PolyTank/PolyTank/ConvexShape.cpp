#include "ConvexShape.h"

using namespace DirectX;

bool ConvexShape::checkIntersection(
	ConvexShape* pOther,
	DirectX::FXMMATRIX thisTransform,
	DirectX::FXMMATRIX otherTransform,
	DirectX::XMVECTOR* pResolution) {

	return false;
}

Box::Box(float width, float height, float depth) : 
	w(width), h(height), d(depth) {}

DirectX::XMVECTOR Box::support(DirectX::FXMVECTOR dir) const {
	return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

DirectX::XMMATRIX Box::inertiaTensor() const {
	return 1.0f / 12.f * XMMatrixSet(
		h * h + d * d,			0.0f,		   0.0f, 0.0f,
				 0.0f, w * w + d * d,		   0.0f, 0.0f,
				 0.0f,			0.0f, w * w + h * h, 0.0f,
				 0.0f,			0.0f,		   0.0f, 0.0f
	);
}
