#include "RigidBody.h"

RigidBody::RigidBody(std::unique_ptr<ConvexShape>&& pShape) : 
	pShape(std::move(pShape)) {}
