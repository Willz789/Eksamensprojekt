#pragma once

#include "ConvexShape.h"
#include "TransformedShape.h"

#include <vector>

class Polyhedron : public ConvexShape, public TransformedShape {
public:
	Polyhedron(DirectX::XMFLOAT3* pVertices, size_t vertexCount);

	std::unique_ptr<TransformedShape> transform(DirectX::FXMMATRIX m) const override;
	DirectX::XMMATRIX inertiaTensor() const override;

	DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const override;
	AABB getBoundingBox() const override;

private:
	std::vector<DirectX::XMFLOAT3> vertices;

};

