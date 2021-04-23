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

Tank::~Tank() {
	if (pRB) {
		PolyTank::get().getPcs().deleteBody(pRB);
	}
	
	if (pNode) {
		pNode->deleteNode();
	}


}

void Tank::update(float dt) {
	XMMATRIX world = pNode->localToWorld();
	XMStoreFloat3(&forwardDir, -world.r[2]);
	XMStoreFloat3(&upDir, world.r[1]);
	XMStoreFloat3(&rightDir, world.r[0]);
	driveForwards();
	turnRight();

	XMVECTOR vel = pRB->getLinMoment()*pRB->getInvMass();
	XMVECTOR dragLin = -XMVector3Length(vel) * vel * dragConstant;
	pRB->addForce(dragLin);
	XMVECTOR angVel = XMVector3Transform(pRB->getAngMoment(), pRB->getInvInertia());
	XMVECTOR dragAng = -XMVector3Length(angVel) * angVel * dragConstant;
	pRB->addTorque(dragAng);

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

void Tank::driveForwards()
{
	pRB->addForce(pRB->getMass() * acc * XMLoadFloat3(&forwardDir));
}

void Tank::driveBackwards()
{
	pRB->addForce(pRB->getMass() * acc * -XMLoadFloat3(&forwardDir));
}

void Tank::turnRight()
{
	//pRB->addForce(0.5 * pRB->getMass() * acc * XMLoadFloat3(&forwardDir), pRB->getPosition() - (boxDims.x / 2) * XMLoadFloat3(&rightDir));
	pRB->addTorque(-acc * XMLoadFloat3(&upDir));
}

void Tank::turnLeft()
{
	//pRB->addForce(0.5 * pRB->getMass() * acc * XMLoadFloat3(&forwardDir), pRB->getPosition() + (boxDims.x / 2) * XMLoadFloat3(&rightDir));
	pRB->addTorque(+acc * XMLoadFloat3(&upDir));
}

DirectX::XMMATRIX Tank::bodyToWorld() {
	return pNode->localToWorld();
}

DirectX::XMMATRIX Tank::turretToWorld() {
	return pNode->getChild(turretNodeIdx)->localToWorld();
}
