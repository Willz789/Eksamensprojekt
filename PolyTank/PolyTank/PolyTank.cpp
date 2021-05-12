#include "PolyTank.h"
#include "GLTFLoader.h"

#include "Interaction.h"
#include "Box.h"
#include "StaticBody.h"

#include <random>
#include <math.h>
#include <sstream>

using namespace DirectX;

PolyTank::PolyTank() :
	player(),
	menu(gfx, wnd.getInteraction()),
	hud(gfx, wnd.getInteraction()),
	state(State::MENU),
	level(),
	roundIdx(0){
	toMenu();
}

PolyTank& PolyTank::get()
{
	static PolyTank instance;
	return instance;
}

void PolyTank::update(float t, float dt) {
	if (state==State::MENU) {
		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

	} else if(state==State::GAME) {
		if (enemiesLeft <= 0) {
			startRound(++roundIdx);
		}
		level.update(t, dt);
		player.update(gfx, pcs, dt);
		gfx.setCamera(player.getCamera()->viewMatrix());

		if (player.isTankDead()) {
			toMenu();
		}
	}
}

void PolyTank::render() {
	gfx.shadowPass(XMLoadFloat3(&scene.lighting.sun.direction));
	scene.draw(gfx);

	gfx.viewPass();
	scene.draw(gfx);
	
	if (state == State::MENU) {
		menu.draw(gfx);
	} else if (state == State::GAME) {
		hud.draw(gfx);
	}
}

void PolyTank::startGame()
{
	state = State::GAME;

	scene.getRoot()->reset();

	wnd.getInteraction()->setCursorLocked(true);
	wnd.getInteraction()->setCursorVisible(false);
	
	player = Player(*wnd.getInteraction());

	menu.removeListeners();
	player.initListeners(gfx, pcs);

	roundIdx = 0;
	startRound(roundIdx);
}

void PolyTank::startRound(uint32_t roundIndex)
{
	gameObjects.clear();
	level.clear();
	int mapId = roundIndex % 4;
	std::string mapName = (std::stringstream() << "./Levels/level" << mapId << ".bin").str();
	level.loadFile(gfx, pcs, mapName, scene);
	player.generateNewTank(gfx, pcs, level, scene.getRoot());
	enemiesLeft = roundIndex + 1;
	
	for (uint32_t i = 0; i < enemiesLeft; i++) {
		emplaceGameObject<Enemy>(gfx, pcs, scene.getRoot(), player.getTank(), level, level.getRandomPathableNode());
	}

	pcs.update(0.0f, 0.0f);
	resetTime();
}

void PolyTank::toMenu()
{
	state = State::MENU;

	gameObjects.clear();
	level.clear();

	assert(scene.getRoot()->isLeaf());
	assert(pcs.bodyCount() == 0);

	wnd.getInteraction()->setCursorLocked(false);
	wnd.getInteraction()->setCursorVisible(true);

	player.removeListeners();
	menu.initListeners();

	gfx.setCamera(XMMatrixLookAtRH(
		XMVectorSet(0.0f, 1.0f, 5.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());



	resetTime();
}

Player& PolyTank::getPlayer()
{
	return player;
}

void PolyTank::enemyDied()
{
	enemiesLeft--;
}
 