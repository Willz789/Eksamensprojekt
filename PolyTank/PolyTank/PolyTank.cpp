#include "PolyTank.h"
#include "GLTFLoader.h"

#include "Interaction.h"

#include <random>
#include <math.h>

using namespace DirectX;

PolyTank::PolyTank() :
	scene(gfx),
	menu(gfx, *this),
	hud(gfx, wnd.getInteraction()),
	state(State::MENU),
	level()
	{
	cameraPos = { 0, 1.0f, 5.0f, 1.0f };
	gfx.setCamera(XMMatrixLookAtRH(XMLoadFloat4(&cameraPos), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());
	
	//pFire = dynamic_cast<Fire*>(scene.getRoot()->addDrawable(std::make_unique<Fire>(gfx, scene.getRoot()->getChild(4)->getChild(11))));
	
	pExplosionNode = scene.getRoot()->addChild();
	pExplosion = dynamic_cast<Explosion*>(scene.getRoot()->addDrawable(std::make_unique<Explosion>(gfx, pExplosionNode)));
	pExplosionNode->translate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	
	pListener = wnd.getInteraction()->addListener([this](const KeyEvent& e) -> void {
		if (e.key == ' ') {
			//pExplosion->run();
		}
	});

}

void PolyTank::update(float t, float dt) {
	if (state==State::MENU) {
		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

	}
	else if(state==State::GAME) {
		XMMATRIX cameraInv = XMMatrixInverse(nullptr, gfx.getCamera());
		if (wnd.getInteraction()->keyDown('A')) {
			XMStoreFloat4(&cameraPos, dt * XMVector4Transform(XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		if (wnd.getInteraction()->keyDown('D')) {
			XMStoreFloat4(&cameraPos, dt * XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		if (wnd.getInteraction()->keyDown('W')) {
			XMStoreFloat4(&cameraPos, dt*XMVector4Transform(XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		if (wnd.getInteraction()->keyDown('S')) {
			XMStoreFloat4(&cameraPos, dt * XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		if (wnd.getInteraction()->keyDown(' ')) {
			XMStoreFloat4(&cameraPos, dt * XMVector4Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		if (wnd.getInteraction()->keyDown(VK_SHIFT)) {
			XMStoreFloat4(&cameraPos, dt * XMVector4Transform(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), cameraInv) + XMLoadFloat4(&cameraPos));
		}

		gfx.setCamera(XMMatrixLookAtRH(XMLoadFloat4(&cameraPos), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
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
	level = Level(gfx, "./Levels/level1.bin", scene);
}
