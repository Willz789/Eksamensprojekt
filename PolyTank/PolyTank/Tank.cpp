#include "Tank.h"

#include "GLTFLoader.h"
#include "Box.h"

using namespace DirectX;

Tank::Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, Interaction& interaction, Tank* pDest) {

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	turretAngle = 0.0f;
	projExist = false;

	pRB = pcs.addBody(std::make_unique<RigidBody>(
		std::make_unique<Box>(1.36f, 0.75f, 2.0f),
		10.0f,
		initPos,
		XMQuaternionIdentity()
	));

	pRB->addAngMoment(XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f));

	interaction.addListener([&gfx, &pcs, pDest](const MouseEvent& e) -> void {
		if (e.button == MouseEvent::Button::RIGHT) {
			pDest->shoot(gfx, pcs);
		}
	});
}

void Tank::update(float dt) {
	pNode->setRotation(pRB->getRotation());
	pNode->setTranslation(pRB->getPosition() - XMVectorSet(0.0f, 0.75f / 2.0f, 0.0f, 0.0f));
	pNode->getChild(turretNodeIdx)->setRotation(XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), turretAngle));
	if (projExist) {
		proj.update(dt);
	}
}

void Tank::shoot(Graphics& gfx, Physics& pcs)
{
	XMMATRIX turretTransform = pNode->getChild(turretNodeIdx)->localToWorld();
	XMVECTOR turretTip = XMVector4Transform(XMVectorSet(0.0f, 0.1f, -2.0f, 1.0f), turretTransform);

	proj = Projectile(gfx, pcs, pNode->getParent(), turretTip, XMQuaternionRotationMatrix(turretTransform), 4);

	projExist = true;
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
