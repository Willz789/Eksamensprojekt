#include "Physics.h"

#include "RigidBody.h"
#include "StaticBody.h"

#include <chrono>
#include <algorithm>

using namespace DirectX;

Body* Physics::addBody(std::unique_ptr<Body>&& pBody, CollisionHandler ch) {
	bodies.push_back(std::move(pBody));
	Body* pb = bodies.back().get();

	if (ch) {
		collisionHandlers[pb] = ch;
	}

	xSorted.push_back({ pb, true });
	xSorted.push_back({ pb, false });

	return pb;
}

void Physics::deleteBody(Body* pBody) {
	
	xSorted.erase(std::remove_if(
		xSorted.begin(),
		xSorted.end(),
		[pBody](EndPoint& ep) -> bool { return ep.pBody == pBody; }
	), xSorted.end());

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

size_t Physics::bodyCount() const
{
	return bodies.size();
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
		RigidBody* pRB = dynamic_cast<RigidBody*>(pBody.get());
		if (pRB) {
			pRB->update(dt);
		}
	}

}

inline bool overlaps(float min1, float max1, float min2, float max2) {
	return !(max1 < min2 || min1 > max2);
}

void Physics::collisions() {
	
	for (size_t i = 0; i < bodies.size(); i++) {
		bodies[i]->updateWorldShape();
	}

	sortAll();

	std::vector<Body*> active;
	std::vector<std::pair<Body*, Body*>> possibleCollisions;
	
	//for (auto it1 = bodies.begin(); it1 != bodies.end(); it1++) {
	//	for (auto it2 = it1 + 1; it2 != bodies.end(); it2++) {
	//		possibleCollisions.push_back({ it1->get(), it2->get() });
	//	}
	//}
	for (const EndPoint& ep : xSorted) {
		if (ep.isBegin) {
			for (Body* pOther : active) {

				if (dynamic_cast<StaticBody*>(ep.pBody) && dynamic_cast<StaticBody*>(pOther)) {
					continue;
				}

				const AABB& bb1 = pOther->getBoundingBox();
				const AABB& bb2 = ep.pBody->getBoundingBox();

				
				if (overlaps(bb1.min.y, bb1.max.y, bb2.min.y, bb2.max.y) &&
					overlaps(bb1.min.z, bb1.max.z, bb2.min.z, bb2.max.z)) {
					possibleCollisions.emplace_back(pOther, ep.pBody);
				}
			}
			active.push_back(ep.pBody);
		} else {
			active.erase(std::find(active.begin(), active.end(), ep.pBody));
		}
	}

	assert(active.empty());

	for (auto pair : possibleCollisions) {

		XMVECTOR resolution;

		if (pair.first->checkCollision(*pair.second, &resolution)) {

			auto it = collisionHandlers.find(pair.first);
			if (it != collisionHandlers.end()) {
				it->second(pair.second, -resolution);
				pair.first->updateWorldShape();
			}

			it = collisionHandlers.find(pair.second);
			if (it != collisionHandlers.end()) {
				it->second(pair.first, resolution);
				pair.second->updateWorldShape();
			}
		}

	}
}

void Physics::sortAll()
{
	/*sort(xSorted, [](const EndPoint& ep) -> float {
		return ep.isBegin ? ep.pBody->getBoundingBox().min.x : ep.pBody->getBoundingBox().max.x;
	});
	*/

	std::sort(xSorted.begin(), xSorted.end(), [](EndPoint& ep1, EndPoint& ep2) -> bool {
		return (ep1.isBegin ? ep1.pBody->getBoundingBox().min.x : ep1.pBody->getBoundingBox().max.x) <
			(ep2.isBegin ? ep2.pBody->getBoundingBox().min.x : ep2.pBody->getBoundingBox().max.x);
	});

}

void Physics::sort(std::vector<EndPoint>& endPoints, std::function<float(const EndPoint&)> eval)
{
	for (auto it = endPoints.begin() + 1; it != endPoints.end(); it++) {
		for (auto it2 = it; it2 != endPoints.begin(); it2--) {
			if (eval(*(it2 - 1)) > eval(*it2)) {
				std::swap(*(it2 - 1), *(it2));
			} else {
				break;
			}
		}
	}

}
