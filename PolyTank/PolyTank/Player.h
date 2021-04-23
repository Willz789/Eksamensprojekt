#pragma once
#include "Interaction.h"
#include "Camera.h"

class Player
{
public:
	Player() = default;
	Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction);
	~Player();

	void initListeners(Graphics& gfx, Physics& pcs);

	Camera* getCamera();
	
private:

	Camera camera;

	Tank* pTank;

	MouseListener* pMListener;
	KeyListener* pKListener;

	Interaction* pInteraction;

	

};