#include "Box.h"

using namespace DirectX;

Box::Box(float width, float height, float depth) :
	w(width), h(height), d(depth) {}

std::unique_ptr<TransformedShape> Box::transform(DirectX::FXMMATRIX m) const {
	return std::make_unique<TransformedBox>(*this, m);
}

DirectX::XMMATRIX Box::inertiaTensor() const {
	return 1.0f / 12.f * XMMatrixSet(
		h * h + d * d, 0.0f, 0.0f, 0.0f,
		0.0f, w * w + d * d, 0.0f, 0.0f,
		0.0f, 0.0f, w * w + h * h, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
}

TransformedBox::TransformedBox(const Box& box, DirectX::FXMMATRIX transform) {
	float x = box.w / 2.0f;
	float y = box.h / 2.0f;
	float z = box.d / 2.0f;

	XMStoreFloat3(&corners[0], XMVector4Transform(XMVectorSet(+x, +y, +z, 1.0f), transform));
	XMStoreFloat3(&corners[1], XMVector4Transform(XMVectorSet(+x, +y, -z, 1.0f), transform));
	XMStoreFloat3(&corners[2], XMVector4Transform(XMVectorSet(+x, -y, +z, 1.0f), transform));
	XMStoreFloat3(&corners[3], XMVector4Transform(XMVectorSet(+x, -y, -z, 1.0f), transform));
	XMStoreFloat3(&corners[4], XMVector4Transform(XMVectorSet(-x, +y, +z, 1.0f), transform));
	XMStoreFloat3(&corners[5], XMVector4Transform(XMVectorSet(-x, +y, -z, 1.0f), transform));
	XMStoreFloat3(&corners[6], XMVector4Transform(XMVectorSet(-x, -y, +z, 1.0f), transform));
	XMStoreFloat3(&corners[7], XMVector4Transform(XMVectorSet(-x, -y, -z, 1.0f), transform));
}

DirectX::XMVECTOR TransformedBox::support(DirectX::FXMVECTOR dir) const {
	XMVECTOR bestCorner;
	float highestDot = -std::numeric_limits<float>::infinity();

	for (auto& corner : corners) {
		XMVECTOR c = XMLoadFloat3(&corner);
		float dot = XMVectorGetX(XMVector3Dot(dir, c));

		if (dot > highestDot) {
			bestCorner = c;
			highestDot = dot;
		}
	}

	return bestCorner;
}
