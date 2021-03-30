#include "PolyTank.h"
#include "GLTFLoader.h"

#include <random>

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
	
	pFire = dynamic_cast<Fire*>(scene.getRoot()->addDrawable(std::make_unique<Fire>(gfx, scene.getRoot()->getChild(0)->getChild(11))));
	
	pExplosionNode = scene.getRoot()->addChild();
	pExplosion = dynamic_cast<Explosion*>(scene.getRoot()->addDrawable(std::make_unique<Explosion>(gfx, pExplosionNode)));
	pExplosionNode->translate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	pListener = wnd.getInteraction()->addListener([this](const KeyEvent& e) -> void {
		if (e.key == ' ') {
			pExplosion->run();
		}
	});

}

void PolyTank::update(float t, float dt) {
	if (state==State::MENU) {
		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

		pFire->update(t, dt);
		pExplosion->update(t, dt);

		if (wnd.getInteraction()->keyDown('A')) {
			pExplosionNode->translate(XMVectorSet(-dt, 0.0f, 0.0f, 0.0f));
		}

		if (wnd.getInteraction()->keyDown('D')) {
			pExplosionNode->translate(XMVectorSet(dt, 0.0f, 0.0f, 0.0f));
		}

		if (wnd.getInteraction()->keyDown('W')) {
			pExplosionNode->translate(XMVectorSet(0.0f, 0.0f, -dt, 0.0f));
		}

		if (wnd.getInteraction()->keyDown('S')) {
			pExplosionNode->translate(XMVectorSet(0.0f, 0.0f, dt, 0.0f));
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
}
