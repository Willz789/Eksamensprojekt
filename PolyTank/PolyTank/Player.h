#pragma once
#include "Interaction.h"
#include "Camera.h"

class Player
{
public:
	Player() = default;
	Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction);
	~Player();

	void update(Graphics& gfx, Physics& pcs, float dt);
	void initListeners(Graphics& gfx, Physics& pcs);

	Camera* getCamera();
	
private:
	bool shooting;
	float shotPower;

	Camera camera;

	Tank* pTank;

	MouseListener* pMListener;
	KeyListener* pKListener;

	Interaction* pInteraction;

	

};