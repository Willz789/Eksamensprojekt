#include "Tank.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"

using namespace DirectX;

Tank::Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, Interaction& interaction) {

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	turretAngle = 0.0f;

	pRB = pcs.emplaceBody<RigidBody>(
		std::make_unique<Box>(boxDims.x, boxDims.y, boxDims.z),
		initPos,
		XMQuaternionIdentity(),
		10.0f
	);

	interaction.addListener([this, &gfx, &pcs](const MouseEvent& e) -> void {
		if (e.button == MouseEvent::Button::RIGHT) {
			this->shoot(gfx, pcs);
		}
	});
}

void Tank::update(float dt) {
	pNode->setRotation(pRB->getRotation());
	pNode->setTranslation(pRB->getPosition() - XMVectorSet(0.0f, 0.75f / 2.0f, 0.0f, 0.0f));
	pNode->getChild(turretNodeIdx)->setRotation(XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), turretAngle));
}

void Tank::shoot(Graphics& gfx, Physics& pcs)
{
	XMMATRIX turretTransform = pNode->getChild(turretNodeIdx)->localToWorld();
	XMVECTOR turretTip = XMVector4Transform(XMVectorSet(0.0f, 0.1f, -2.0f, 1.0f), turretTransform);

	PolyTank::get().emplaceGameObject<Projectile>(gfx, pcs, pNode->getParent(), turretTip, XMQuaternionRotationMatrix(turretTransform), 4);
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
