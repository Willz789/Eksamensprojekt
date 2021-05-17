#pragma once

#include "Util.h"

struct AABB {
	DirectX::XMFLOAT3 min, max;
};

class TransformedShape {
public:
	virtual ~TransformedShape() = default;

	virtual DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const = 0;
	virtual AABB getBoundingBox() const = 0;

	bool checkIntersection(const TransformedShape* pOther, DirectX::XMVECTOR* pResolution) const;

};


