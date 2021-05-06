#include "Tank.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"

using namespace DirectX;

Tank::Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos) {

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	turretYaw = 0.0f;
	turretPitch = 0.0f;

	pRB = pcs.emplaceBody<RigidBody>(
		std::make_unique<Box>(boxDims.x, boxDims.y, boxDims.z),
		initPos,
		XMQuaternionIdentity(),
		10.0f
	);

	pcs.assignCollisionHandler(pRB, [this](Body* pOther, FXMVECTOR resolution) -> void {
		if (dynamic_cast<StaticBody*>(pOther)) {
			pRB->move(resolution);
			pRB->addMoment(-XMVectorSwizzle<3, 1, 3, 3>(pRB->getLinMoment()));
			pRB->addForce(-XMVectorSwizzle<3, 1, 3, 3>(pRB->getForce()));
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

	XMVECTOR vel = pRB->getLinMoment()*pRB->getInvMass();
	XMVECTOR dragLin = -XMVector3Length(vel) * vel * dragConstant;
	pRB->addForce(dragLin);

	XMVECTOR angVel = XMVector3Transform(pRB->getAngMoment(), pRB->getInvInertia());
	XMVECTOR dragAng = -XMVector3Length(angVel) * angVel * dragConstant;
	pRB->addTorque(dragAng);

	pNode->setRotation(pRB->getRotation());
	pNode->setTranslation(pRB->getPosition() - XMVectorSet(0.0f, 0.75f / 2.0f, 0.0f, 0.0f));
	pNode->getChild(turretNodeIdx)->setRotation(XMQuaternionRotationRollPitchYaw(turretPitch, turretYaw, 0.0f));

	
}

void Tank::shoot(Graphics& gfx, Physics& pcs, float power)
{
	XMMATRIX turretTransform = pNode->getChild(turretNodeIdx)->localToWorld();
	XMVECTOR turretTip = XMVector4Transform(XMVectorSet(0.0f, 0.1f, -2.0f, 1.0f), turretTransform);
	PolyTank::get().emplaceGameObject<Projectile>(gfx, pcs, pNode->getParent(), turretTip, XMQuaternionRotationMatrix(turretTransform), power);
}

void Tank::rotateTurret(float yaw, float pitch) {
	turretYaw = normalizeAngle(turretYaw + yaw);
	turretPitch = std::clamp(turretPitch + pitch, 0.001f, 0.499f*pi);
}

void Tank::resetTurretPitch()
{
	turretPitch = 0.0f;
}

void Tank::driveForward(float dt)
{

	pRB->addForce(pRB->getMass() * acc * XMLoadFloat3(&forwardDir));
	//pRB->setPosition(pRB->getPosition() + 5.0f * dt * XMLoadFloat3(&forwardDir));
}

void Tank::driveBackward(float dt)
{
	pRB->addForce(pRB->getMass() * acc * -XMLoadFloat3(&forwardDir));
	//pRB->setPosition(pRB->getPosition() - 5.0f * dt * XMLoadFloat3(&forwardDir));
}
 
void Tank::turnRight(float dt)
{
	//pRB->addForce(0.5 * pRB->getMass() * acc * XMLoadFloat3(&forwardDir), pRB->getPosition() - (boxDims.x / 2) * XMLoadFloat3(&rightDir));
	//pRB->addTorque(-acc * XMLoadFloat3(&upDir));
	pRB->setRotation(XMQuaternionMultiply(pRB->getRotation(), XMQuaternionRotationNormal(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), dt)));
}

void Tank::turnLeft(float dt)
{
	//pRB->addForce(0.5 * pRB->getMass() * acc * XMLoadFloat3(&forwardDir), pRB->getPosition() + (boxDims.x / 2) * XMLoadFloat3(&rightDir));
	//pRB->addTorque(+acc * XMLoadFloat3(&upDir));
	pRB->setRotation(XMQuaternionMultiply(pRB->getRotation(), XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dt)));
}

DirectX::XMMATRIX Tank::bodyToWorld() {
	return pNode->localToWorld();
}

DirectX::XMMATRIX Tank::turretToWorld() {
	return pNode->getChild(turretNodeIdx)->localToWorld();
}

DirectX::XMVECTOR Tank::getTurretTipPos()
{
	XMMATRIX turretTransform = pNode->getChild(turretNodeIdx)->localToWorld();
	return XMVector4Transform(XMVectorSet(0.0f, 0.2f, -1.0f, 1.0f), turretTransform);
}

DirectX::XMMATRIX Tank::getTurretTransform()
{
	return pNode->getChild(turretNodeIdx)->localToWorld();;
}

DirectX::XMVECTOR Tank::getPosition()
{
	return pRB->getPosition();
}
