#pragma once

#include "Tank.h"
#include "Level.h"

class Enemy
{
public:
	Enemy(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Tank* pTarget, Level& lvl);

	void update(float dt);
	void buildPath();

private:
	Tank* pTank;
	Tank* pTarget;

	Level* pLvl;
	std::vector<Node> path;

	float shootingRange;
	float shootingCooldown;
};

