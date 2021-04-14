#include "Physics.h"

using namespace DirectX;

void Physics::update(float t, float dt)
{
	for (RigidBody* pr : rigidBodies) {
		XMVECTOR gravity = XMVectorSet(0.0f, pr->mass*-g, 0.0f, 0.0f);
		pr->addForce(gravity);
		pr->update(dt);
	}
}
