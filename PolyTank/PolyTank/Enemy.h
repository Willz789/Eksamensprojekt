#pragma once

#include "Tank.h"
#include "Level.h"

#include <thread>
#include <atomic>

class Enemy : public IGameObject
{
public:
	Enemy(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Tank* pTarget, Level& lvl, Node startNode);

	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy(Enemy&&) = delete;
	Enemy& operator=(Enemy&&) = delete;
	
	~Enemy();

	void update(float dt);
	void move(float dt);
	void aim();
	
	void buildPath();

private:
	static constexpr float shotPower = 25.0f;

	Tank* pTank;
	Tank* pTarget;

	Level* pLvl;
	std::vector<Node> path;

	std::thread pathBuilder;
	std::atomic<bool> pathLocked;

	float shootingRange;
	float shootingCooldown;
};

