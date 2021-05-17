#pragma once
#include "Interaction.h"
#include "Level.h"
#include "Camera.h"

class Player
{
public:
	Player() = default;
	Player(Interaction& interaction);
	~Player(); 

	void update(Graphics& gfx, Physics& pcs, float dt);
	void initListeners(Graphics& gfx, Physics& pcs);
	void removeListeners();

	void generateNewTank(Graphics& gfx, Physics& pcs, Level& lvl, SceneNode* pRoot);

	Tank* getTank();
	Camera* getCamera();

	void setDamage(uint8_t multiplier);
		
	void tankDied();
	bool isTankDead();

	float getShotPower();
	float getMaxShotPower();
	uint8_t getDamageMultiplier();

	uint32_t getPoints();
	void addPoints(uint32_t addedPoints);

private:
	static constexpr int32_t baseDamage = 20;

private:
	bool shooting;
	float shotPower;
	float maxShotPower;
	bool tankDead;

	uint8_t damageMultiplier;

	uint32_t points;

	Camera camera;

	Tank* pTank;

	MouseListener* pMListener;
	Interaction* pInteraction;

};