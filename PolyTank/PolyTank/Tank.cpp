#include "Tank.h"

#include "GLTFLoader.h"
#include "Box.h"

using namespace DirectX;

Tank::Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos) {

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	turretAngle = 0.0f;

	pRB = pcs.addBody(std::make_unique<RigidBody>(
		std::make_unique<Box>(1.36f, 0.75f, 2.0f),
		10.0f,
		initPos,
		XMQuaternionIdentity()
	));

	pRB->addAngMoment(XMVectorSet(0.0f, 0.0f, 4.0f, 0.0f));
}

void Tank::update(Level& lvl, float dt) {
	pNode->setRotation(pRB->getRotation());
	pNode->setTranslation(pRB->getPosition() - XMVectorSet(0.0f, 0.75f / 2.0f, 0.0f, 0.0f));
	pNode->getChild(turretNodeIdx)->setRotation(XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), turretAngle));
}

void Tank::rotateTurret(float angle) {
	turretAngle = normalizeAngle(turretAngle + angle);
}

DirectX::XMMATRIX Tank::bodyToWorld() {
	return pNode->localToWorld();
}

DirectX::XMMATRIX Tank::turretToWorld() {
	return pNode->getChild(turretNodeIdx)->localToWorld();
}
