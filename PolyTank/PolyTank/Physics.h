#pragma once

#include "RigidBody.h"

#include <vector>

class Physics {
public:
	
	RigidBody* addBody(std::unique_ptr<RigidBody>&& pRB);
	void update(float t, float dt);
	
private:	
	static constexpr float g = 9.82f;


private:
	std::vector<std::unique_ptr<RigidBody>> rigidBodies;
	
};

