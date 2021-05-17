#include "Box.h"
#include "Polyhedron.h"

#include <algorithm>


using namespace DirectX;

Box::Box(float width, float height, float depth) :
	w(width), h(height), d(depth) {}

std::unique_ptr<TransformedShape> Box::transform(DirectX::FXMMATRIX m) const {

	float x = w / 2.0f;
	float y = h / 2.0f;
	float z = d / 2.0f;

	XMFLOAT3 corners[8];

	XMStoreFloat3(&corners[0], XMVector4Transform(XMVectorSet(+x, +y, +z, 1.0f), m));
	XMStoreFloat3(&corners[1], XMVector4Transform(XMVectorSet(+x, +y, -z, 1.0f), m));
	XMStoreFloat3(&corners[2], XMVector4Transform(XMVectorSet(+x, -y, +z, 1.0f), m));
	XMStoreFloat3(&corners[3], XMVector4Transform(XMVectorSet(+x, -y, -z, 1.0f), m));
	XMStoreFloat3(&corners[4], XMVector4Transform(XMVectorSet(-x, +y, +z, 1.0f), m));
	XMStoreFloat3(&corners[5], XMVector4Transform(XMVectorSet(-x, +y, -z, 1.0f), m));
	XMStoreFloat3(&corners[6], XMVector4Transform(XMVectorSet(-x, -y, +z, 1.0f), m));
	XMStoreFloat3(&corners[7], XMVector4Transform(XMVectorSet(-x, -y, -z, 1.0f), m));

	return std::make_unique<Polyhedron>(corners, std::size(corners));
}

DirectX::XMMATRIX Box::inertiaTensor() const {
	return 1.0f / 12.f * XMMatrixSet(
		h * h + d * d, 0.0f, 0.0f, 0.0f,
		0.0f, w * w + d * d, 0.0f, 0.0f,
		0.0f, 0.0f, w * w + h * h, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
}
