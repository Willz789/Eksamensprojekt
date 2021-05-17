#pragma once

#include "Body.h"
#include "Util.h"

#include <memory>

class StaticBody : public Body {
public:
	StaticBody(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR pos, DirectX::FXMVECTOR rot, bool terrain = true);

	void move(DirectX::FXMVECTOR translation) override;
	void addForce(DirectX::FXMVECTOR f) override;
	void update(float dt) override;
	float getMass() const override;
	float getInvMass() const override;
	bool isTerrain() const;

private:
	bool terrain;
};
