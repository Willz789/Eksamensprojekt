#include "Tank.h"

#include "GLTFLoader.h"

using namespace DirectX;

Tank::Tank(Graphics& gfx, SceneNode* pRoot, DirectX::FXMVECTOR initPos) {
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	XMStoreFloat3(&position, initPos);
	turretAngle = 0.0f;
}

void Tank::update(Level& lvl, float dt) {
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVectorSet(dt, 0.0f, 0.0f, 0.0f));


	pNode->setTranslation(XMLoadFloat3(&position));
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
