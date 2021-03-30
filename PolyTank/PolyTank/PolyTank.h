#pragma once

#include "Application.h"
#include "Menu.h"
#include "HUD.h"

#include "Fire.h"
#include "Explosion.h"

class PolyTank : public Application {
	enum class State {
		MENU,
		GAME
	};


public:
	PolyTank();

	void update(float t, float dt) override;
	void render() override;

	void startGame();

private:

	State state;
	
	Scene scene;
	Menu menu;
	HUD hud;

	Fire* pFire;
	Explosion* pExplosion;
	SceneNode* pExplosionNode;

	KeyListener* pListener;
};
