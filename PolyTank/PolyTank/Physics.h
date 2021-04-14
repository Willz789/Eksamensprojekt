#pragma once

#include "RigidBody.h"

#include <vector>

class Physics {
public:
	static constexpr float g = 9.82f;
	void update(float t, float dt);

private:
	std::vector<RigidBody*> rigidBodies;
	
};

