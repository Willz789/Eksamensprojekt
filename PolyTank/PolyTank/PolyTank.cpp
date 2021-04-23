#include "PolyTank.h"
#include "GLTFLoader.h"

#include "Interaction.h"
#include "Box.h"
#include "StaticBody.h"

#include <random>
#include <math.h>

using namespace DirectX;

PolyTank::PolyTank() :
	scene(gfx),
	menu(gfx, *this),
	hud(gfx, wnd.getInteraction()),
	state(State::MENU),
	level(),
	camera(wnd.getInteraction()) {

	cameraPos = { 0, 1.0f, 5.0f, 1.0f };
	gfx.setCamera(XMMatrixLookAtRH(XMLoadFloat4(&cameraPos), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());
}

PolyTank& PolyTank::get()
{
	static PolyTank instance;
	return instance;
}

IGameObject* PolyTank::addGameObject(std::unique_ptr<IGameObject>&& pGameObject)
{
	gameObjects.push_back(std::move(pGameObject));
	return gameObjects.back().get();
}

void PolyTank::deleteGameObject(IGameObject* pGameObject)
{
	deleteList.push_back(pGameObject);
}

void PolyTank::update(float t, float dt) {
	if (state==State::MENU) {
		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

	} else if(state==State::GAME) {
		for (auto& g : gameObjects) {
			g->update(dt);
		}
		pcs.update(t, dt);
		
		gfx.setCamera(camera.viewMatrix());
	}

	gameObjects.erase(
		std::remove_if(gameObjects.begin(), gameObjects.end(), [this](std::unique_ptr<IGameObject>& g)->bool{
			return std::find(deleteList.begin(), deleteList.end(), g.get()) != deleteList.end();
		}), gameObjects.end()
	);
	deleteList.clear();
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

Physics& PolyTank::getPcs()
{
	return pcs;
}

void PolyTank::startGame()
{
	state = State::GAME;
	scene.getRoot()->reset();
	wnd.getInteraction()->setCursorLocked(true);
	wnd.getInteraction()->setCursorVisible(false);

	level = Level(gfx, "./Levels/level1.bin", scene);
	
	Tank* pTank = emplaceGameObject<Tank>(gfx, pcs, scene.getRoot(), XMVectorSet(0.0f, 4.0f, 0.0f, 0.0f), *wnd.getInteraction());
	camera.assignTank(*pTank);
	emplaceGameObject<Tank>(gfx, pcs, scene.getRoot(), XMVectorSet(3.0f, 1.0f, 0.0f, 0.0f), *wnd.getInteraction());

	pcs.emplaceBody<StaticBody>(
		std::make_unique<Box>(10.0f, 1.0f, 10.0f),
		XMVectorSet(0.0f, -0.5f, 0.0f, 0.0f),
		XMQuaternionIdentity());
}
