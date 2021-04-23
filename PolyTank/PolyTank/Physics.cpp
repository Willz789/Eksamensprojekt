#include "Physics.h"

#include "RigidBody.h"

using namespace DirectX;

Body* Physics::addBody(std::unique_ptr<Body>&& pBody) {
	bodies.push_back(std::move(pBody));
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

void Physics::update(float t, float dt)
{
	
	for (auto& pBody : bodies) {
		if (RigidBody* pRB = dynamic_cast<RigidBody*>(pBody.get())) {

			XMVECTOR gravity = XMVectorSet(0.0f, pRB->getMass() * -g, 0.0f, 0.0f);
			pRB->addForce(gravity);
			pRB->update(dt);
		}
	}

	collisions();
}

void Physics::collisions() {
	XMVECTOR resolution;

	for (size_t i = 0; i < bodies.size(); i++) {
		for (size_t j = i + 1; j < bodies.size(); j++) {
			
			if (bodies[i]->checkCollision(*bodies[j].get(), &resolution)) {

				// resolve impulse
				if (RigidBody* pRB = dynamic_cast<RigidBody*>(bodies[i].get())) {
					XMVECTOR linMom = pRB->getLinMoment();
					pRB->addMoment(XMVectorSet(0.0f, -XMVectorGetY(linMom), 0.0f, 0.0f));
				}

				// resolve positions
				float invMassi = bodies[i]->getInvMass();
				float invMassj = bodies[j]->getInvMass();
				float invMassSum = invMassi + invMassj;

				bodies[i]->move(-(invMassi / invMassSum) * resolution);
				bodies[j]->move(+(invMassj / invMassSum) * resolution);

			}
		}
	}
}
