#include "PolyTank.h"
#include "GLTFLoader.h"

using namespace DirectX;

PolyTank::PolyTank() :
	scene(gfx),
	menu(gfx, *this),
	hud(gfx, wnd.getInteraction()),
	state(State::MENU)
	{

	gfx.setCamera(XMMatrixLookAtRH(XMVectorSet(0, 1.0f, 5.0f, 1.0f), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());
}

void PolyTank::update(float t, float dt) {
	if (state==State::MENU) {
		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));
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
}
