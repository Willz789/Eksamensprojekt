#pragma once

#include "ConvexShape.h"

#include <memory>

class RigidBody {
public:
	RigidBody(std::unique_ptr<ConvexShape>&& pShape);

private:
	std::unique_ptr<ConvexShape> pShape;
};
