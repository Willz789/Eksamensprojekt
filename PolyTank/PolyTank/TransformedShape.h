#pragma once

#include "Util.h"

class TransformedShape {
public:
	virtual ~TransformedShape() = default;

	virtual DirectX::XMVECTOR support(DirectX::FXMVECTOR dir) const = 0;
	
	bool checkIntersection(const TransformedShape* pOther, DirectX::XMVECTOR* pResolution) const;

};


