#pragma once

#include "Body.h"
#include "Util.h"

#include <memory>

class StaticBody : public Body {
public:
	StaticBody(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR pos, DirectX::FXMVECTOR rot);

	void move(DirectX::FXMVECTOR translation) override;
	float getInvMass() const override;
};
