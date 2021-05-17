#pragma once

#include "Application.h"
#include "Menu.h"
#include "HUD.h"
#include "Level.h"
#include "Tank.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"

#include <memory>
#include <utility>

class PolyTank : public Application {
	enum class State {
		MENU,
		GAME
	};

public:
	PolyTank();

	static PolyTank& get();

	void update(float t, float dt) override;
	void render() override;

	void startGame();
	void startRound(uint32_t roundIndex);
	void toMenu();

	Player& getPlayer();

	void enemyDied();

private:
	uint32_t roundIdx;
	uint32_t enemiesLeft;

	State state;
	Menu menu;
	HUD hud;

	Player player;
	Level level;
};
