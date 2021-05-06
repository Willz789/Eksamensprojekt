#pragma once

#include "Body.h"

#include <vector>
#include <unordered_map>
#include <functional>

using CollisionHandler = std::function<void(Body*, DirectX::FXMVECTOR)>;

struct EndPoint {
	Body* pBody;
	bool isBegin;
};

class Physics {
public:
	
	Body* addBody(std::unique_ptr<Body>&& pBody, CollisionHandler ch = nullptr);
	template<typename T, typename... CTorArgs>
	T* emplaceBody(CTorArgs&&... args);
	void deleteBody(Body* pBody);
	void assignCollisionHandler(Body* pBody, CollisionHandler ch);

	void update(float t, float dt);
	
private:
	void collisions();
	void sortAll();
	void sort(std::vector<EndPoint>& endPoints, std::function<float(const EndPoint&)> eval);

private:	
	static constexpr float g = 9.82f;

private:
	std::vector<std::unique_ptr<Body>> bodies;
	std::unordered_map<Body*, CollisionHandler> collisionHandlers;

	std::vector<EndPoint> xSorted;
};

template<typename T, typename ...CTorArgs>
inline T* Physics::emplaceBody(CTorArgs && ...args) {
	bodies.push_back(std::make_unique<T>(std::forward<CTorArgs>(args)...));

	Body* pb = bodies.back().get();

	xSorted.push_back({ pb, true });
	xSorted.push_back({ pb, false });
	
	return static_cast<T*>(pb);
}
