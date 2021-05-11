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
		
	void tankDied();
	bool isTankDead();

private:
	bool shooting;
	float shotPower;
	bool tankDead;

	Camera camera;

	Tank* pTank;

	MouseListener* pMListener;
	Interaction* pInteraction;

};