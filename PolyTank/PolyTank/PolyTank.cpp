#include "PolyTank.h"
#include "GLTFLoader.h"

#include <random>

using namespace DirectX;

PolyTank::PolyTank() :
	scene(gfx),
	menu(gfx),
	fire(gfx, 0.01f, 256) {
	
	gfx.setCamera(XMMatrixLookAtRH(XMVectorSet(0, 1.0f, 5.0f, 1.0f), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());

	SceneNode* pFireNode = scene.getRoot()->addChild();
	pFireNode->translate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	pFireNode->getMesh()->addDrawable(fire.makeDrawable(gfx));

}

void PolyTank::update(float t, float dt) {
	scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
	scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

	fire.update(gfx, t, dt);
}

void PolyTank::render() {
	gfx.shadowPass(XMLoadFloat3(&scene.lighting.sun.direction));
	scene.draw(gfx);

	gfx.viewPass();
	scene.draw(gfx);

	menu.draw(gfx);
}
