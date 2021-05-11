#pragma once
#include "Interaction.h"
#include "Level.h"
#include "Camera.h"

class Player
{
public:
	Player() = default;
	Player(Graphics& gfx, Physics& pcs, Level& lvl, SceneNode* pRoot, Interaction& interaction);
	~Player(); 

	void update(Graphics& gfx, Physics& pcs, float dt);
	void initListeners(Graphics& gfx, Physics& pcs);
	void removeListeners();

	Tank* getTank();
	Camera* getCamera();

	uint8_t getActivePowerUp();

	void setDamage(float multiplier);
		
	void tankDied();
	bool isTankDead();

	float getShotPower();
	float getMaxShotPower();

private:
	static constexpr int32_t baseDamage = 20;

private:
	bool shooting;
	float shotPower;
	float maxShotPower;
	bool tankDead;

	float damageMultiplier;

	uint8_t activePowerUp;

	Camera camera;

	Tank* pTank;

	MouseListener* pMListener;
	Interaction* pInteraction;

};