#include "Physics.h"

using namespace DirectX;

RigidBody* Physics::addBody(std::unique_ptr<RigidBody>&& pRB) {
	rigidBodies.push_back(std::move(pRB));
	return rigidBodies.back().get();
}

void Physics::update(float t, float dt)
{
	for (auto& pr : rigidBodies) {
		XMVECTOR gravity = XMVectorSet(0.0f, pr->mass * -g, 0.0f, 0.0f);
		pr->addForce(gravity);
		pr->update(dt);
	}
}
