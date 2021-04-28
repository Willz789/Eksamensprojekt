#include "Polyhedron.h"

#include <memory>

using namespace DirectX;

Polyhedron::Polyhedron(DirectX::XMFLOAT3* pVertices, size_t vertexCount) :
	vertices(vertexCount) {	
	std::copy(pVertices, pVertices + vertexCount, vertices.begin());
}

std::unique_ptr<TransformedShape> Polyhedron::transform(DirectX::FXMMATRIX m) const {
	auto pTransformed = std::make_unique<Polyhedron>(nullptr, 0);
	pTransformed->vertices.resize(vertices.size());

	for (size_t i = 0; i < vertices.size(); i++) {
		XMVECTOR v = XMVectorSetW(XMLoadFloat3(&vertices[i]), 1.0f);
		XMVECTOR vt = XMVector4Transform(v, m);
		XMStoreFloat3(&pTransformed->vertices[i], vt);
	}

	return pTransformed;
}

DirectX::XMMATRIX Polyhedron::inertiaTensor() const {
	assert(false);
	return XMMatrixIdentity();
}

DirectX::XMVECTOR Polyhedron::support(DirectX::FXMVECTOR dir) const {
	XMVECTOR max = XMVectorZero();
	float maxDot = -std::numeric_limits<float>::infinity();

	for (const XMFLOAT3& p : vertices) {
		XMVECTOR v = XMLoadFloat3(&p);
		float dot = XMVectorGetX(XMVector3Dot(dir, v));
		if (dot > maxDot) {
			max = v;
			maxDot = dot;
		}
	}

	return max;
}

AABB Polyhedron::getBoundingBox() const {
	AABB bb;
	bb.max = { vertices[0].x, vertices[0].y, vertices[0].z };
	bb.min = bb.max;

	for (auto it = vertices.begin() + 1; it != vertices.end(); it++) {
		bb.max.x = std::max(it->x, bb.max.x);
		bb.max.y = std::max(it->y, bb.max.y);
		bb.max.z = std::max(it->z, bb.max.z);

		bb.min.x = std::min(it->x, bb.min.x);
		bb.min.y = std::min(it->y, bb.min.y);
		bb.min.z = std::min(it->z, bb.min.z);
	}

	return bb;
}
