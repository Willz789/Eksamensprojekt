#pragma once

#include "Body.h"

#include <vector>
#include <unordered_map>
#include <functional>

using CollisionHandler = std::function<void(Body*, DirectX::FXMVECTOR)>;

class Physics {
public:
	
	Body* addBody(std::unique_ptr<Body>&& pBody, CollisionHandler ch = nullptr);
	template<typename T, typename... CTorArgs>
	T* emplaceBody(CTorArgs&&... args);
	void deleteBody(Body* pBody);
	void assignCollisionHandler(Body* pBody, CollisionHandler ch);

	void update(float t, float dt);
	void collisions();
	
private:	
	static constexpr float g = 9.82f;

private:
	std::vector<std::unique_ptr<Body>> bodies;
	std::unordered_map<Body*, CollisionHandler> collisionHandlers;

};

template<typename T, typename ...CTorArgs>
inline T* Physics::emplaceBody(CTorArgs && ...args) {
	bodies.push_back(std::make_unique<T>(std::forward<CTorArgs>(args)...));
	return static_cast<T*>(bodies.back().get());
}
