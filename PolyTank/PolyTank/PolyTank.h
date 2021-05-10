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

	IGameObject* addGameObject(std::unique_ptr<IGameObject>&& pGameObject);
	template <typename T, typename ...CTorArgs>
	T* emplaceGameObject(CTorArgs&&... args);

	void deleteGameObject(IGameObject* pGameObject);

	void update(float t, float dt) override;
	void render() override;

	Physics& getPcs();

	void startGame();

private:

	State state;
	
	Scene scene;
	Menu menu;
	HUD hud;

	Player player;

	Level level;

	std::vector<std::unique_ptr<IGameObject>> gameObjects;
	std::vector<IGameObject*> deleteList;

	
};

template<typename T, typename ...CTorArgs>
inline T* PolyTank::emplaceGameObject(CTorArgs&&... args)
{
	gameObjects.push_back(std::make_unique<T>(std::forward<CTorArgs>(args)...));
	return static_cast<T*>(gameObjects.back().get());
}
