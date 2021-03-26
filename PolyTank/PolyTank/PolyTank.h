#pragma once

#include "Application.h"

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

};

