#pragma once

#include "Body.h"

#include <vector>

class Physics {
public:
	
	Body* addBody(std::unique_ptr<Body>&& pRB);
	template<typename T, typename... CTorArgs>
	T* emplaceBody(CTorArgs&&... args);
	void deleteBody(Body* pRB);

	void update(float t, float dt);
	void collisions();
	
private:	
	static constexpr float g = 9.82f;

private:
	std::vector<std::unique_ptr<Body>> bodies;

};

template<typename T, typename ...CTorArgs>
inline T* Physics::emplaceBody(CTorArgs && ...args) {
	bodies.push_back(std::make_unique<T>(std::forward<CTorArgs>(args)...));
	return static_cast<T*>(bodies.back().get());
}
