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
		//pr->addForce(gravity);
		pr->update(dt);
	}

	for (size_t i = 0; i < rigidBodies.size(); i++) {
		for (size_t j = i + 1; j < rigidBodies.size(); j++) {
		
			if (rigidBodies[i]->checkCollision(*rigidBodies[j].get(), nullptr)) {
				std::cout << "1";
			} else {
				std::cout << "0";
			}

		}
	}

}