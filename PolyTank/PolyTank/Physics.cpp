#include "Physics.h"

#include "RigidBody.h"
#include "StaticBody.h"

#include <chrono>

using namespace DirectX;

Body* Physics::addBody(std::unique_ptr<Body>&& pBody, CollisionHandler ch) {
	bodies.push_back(std::move(pBody));
	if (ch) {
		collisionHandlers[bodies.back().get()] = ch;
	}
	return bodies.back().get();
}

void Physics::deleteBody(Body* pBody) {
	for (auto it = bodies.begin(); it != bodies.end(); it++) {
		if (pBody == it->get()) {
			bodies.erase(it);
			break;
		}
	}
}

void Physics::assignCollisionHandler(Body* pBody, CollisionHandler ch) {
	collisionHandlers[pBody] = ch;
}

void Physics::update(float t, float dt)
{
	for (auto& pBody : bodies) {
		if (RigidBody* pRB = dynamic_cast<RigidBody*>(pBody.get())) {
			XMVECTOR gravity = XMVectorSet(0.0f, pRB->getMass() * -g, 0.0f, 0.0f);
			pRB->addForce(gravity);
		}
	}

	collisions();

	for (auto& pBody : bodies) {
		if (RigidBody* pRB = dynamic_cast<RigidBody*>(pBody.get())) {
			pRB->update(dt);
		}
	}

}

void Physics::collisions() {
	XMVECTOR resolution;

	auto t0 = std::chrono::steady_clock::now();

	for (size_t i = 0; i < bodies.size(); i++) {
		bodies[i]->updateWorldShape();
	}

	for (size_t i = 0; i < bodies.size(); i++) {
		for (size_t j = i + 1; j < bodies.size(); j++) {
			
			if (dynamic_cast<StaticBody*>(bodies[i].get()) &&
				dynamic_cast<StaticBody*>(bodies[j].get())) {
				continue;
			}

			if (bodies[i]->checkCollision(*bodies[j].get(), &resolution)) {

				auto it = collisionHandlers.find(bodies[i].get());
				if (it != collisionHandlers.end()) {
					it->second(bodies[j].get(), resolution);
					bodies[i]->updateWorldShape();
				}

				it = collisionHandlers.find(bodies[j].get());
				if (it != collisionHandlers.end()) {
					it->second(bodies[i].get(), resolution);
					bodies[j]->updateWorldShape();
				}
			}
		}
	}

	auto t1 = std::chrono::steady_clock::now();

	uint64_t delta = (t1 - t0).count();

	std::cout << delta / 1e6f << "\n";
}
