#pragma once

#include "RigidBody.h"

#include <vector>

class Physics {
public:
	void update(float t, float dt);

private:
	
private:
	std::vector<RigidBody*> rigidBodies;
};

