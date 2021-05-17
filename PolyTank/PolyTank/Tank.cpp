#include "Tank.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"
#include "GLTFMaterial.h"
#include "Projectile.h"

using namespace DirectX;

void setColor(SceneNode* pNode, FXMVECTOR color) {
	
	for (size_t i = 0; i < pNode->drawableCount(); i++) {
		Mesh* pMesh = dynamic_cast<Mesh*>(pNode->getDrawable(i));
		if (!pMesh) continue;
		auto pMat = pMesh->getBindableByType<GLTFMaterial>();
		if (!pMat) continue;
		XMStoreFloat3(&pMat->factors.baseColor, color);
	}

	for (size_t i = 0; i < pNode->childCount(); i++) {
		setColor(pNode->getChild(i), color);
	}
}

Tank::Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, FXMVECTOR initPos, FXMVECTOR color) :
	turretYaw(0.0f),
	turretPitch(0.0f),
	maxHealth(100),
	health(100) {

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	setColor(pNode, color);

	pRB = pcs.emplaceBody<RigidBody>(
		std::make_unique<Box>(boxDims.x, boxDims.y, boxDims.z),
		initPos + XMVectorSet(0.0f, boxDims.y, 0.0f, 0.0f),
		XMQuaternionIdentity(),
		10.0f,
		this
	);

	pcs.assignCollisionHandler(pRB, [this](Body* pOther, FXMVECTOR resolution) -> void {
		auto* pSB = dynamic_cast<StaticBody*>(pOther);
		if (pSB && pSB->isTerrain()) {
			pRB->move(resolution);
			pRB->addMoment(-XMVectorSwizzle<3, 1, 3, 3>(pRB->getLinMoment()));
			pRB->addForce(-XMVectorSwizzle<3, 1, 3, 3>(pRB->getForce()));
		}
	});

	update(0.0f);
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

	pNode->setRotation(pRB->getRotation());
	pNode->setTranslation(pRB->getPosition());
	pNode->getChild(turretNodeIdx)->setRotation(XMQuaternionRotationRollPitchYaw(turretPitch, turretYaw, 0.0f));

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
	
	if (XMVectorGetY(pRB->getPosition()) <= -5) {
		takeDamage(1);
	}
}

void Tank::shoot(Graphics& gfx, Physics& pcs, float power, int32_t damage)
{
	XMMATRIX turretTransform = pNode->getChild(turretNodeIdx)->localToWorld();

	PolyTank::get().emplaceGameObject<Projectile>(gfx, pcs, pNode->getParent(), getTurretTipPos(), XMQuaternionRotationMatrix(turretTransform), power, damage, this);
}

void Tank::rotateTurret(float yaw, float pitch) {
	turretYaw = normalizeAngle(turretYaw + yaw);
	turretPitch = std::clamp(turretPitch + pitch, -0.1f, 0.4999f*pi);
}

void Tank::setTurretRotation(float yaw, float pitch)
{
	turretYaw = yaw;
	turretPitch = pitch;
}

void Tank::resetTurretPitch()
{
	turretPitch = 0.0f;
}

void Tank::driveForward(float dt)
{
	pRB->setPosition(pRB->getPosition() + 5.0f * dt * XMLoadFloat3(&forwardDir));
}

void Tank::driveBackward(float dt)
{
	pRB->setPosition(pRB->getPosition() - 5.0f * dt * XMLoadFloat3(&forwardDir));
}
 
void Tank::turnRight(float dt)
{
	pRB->setRotation(XMQuaternionMultiply(pRB->getRotation(), XMQuaternionRotationNormal(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), rotationSpeed * dt)));
}

void Tank::turnLeft(float dt)
{
	pRB->setRotation(XMQuaternionMultiply(pRB->getRotation(), XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationSpeed * dt)));
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
	return XMVector4Transform(XMVectorSet(0.0f, 0.05f, -1.0f, 1.0f), turretTransform);
}

DirectX::XMVECTOR Tank::getPosition()
{
	return pRB->getPosition();
}

DirectX::XMVECTOR Tank::getGroundPosition()
{
	return pRB->getPosition() - XMVectorSet(0.0f, boxDims.y / 2.0f, 0.0f, 0.0f);
}

float Tank::getRotationSpeed()
{
	return rotationSpeed;
}

void Tank::takeDamage(int32_t damage)
{
	health -= damage;

	if (onDeath && health <= 0) {
		onDeath();
	}
}

void Tank::heal(int32_t addedHealth)
{
	health += addedHealth;
	if (health > maxHealth) {
		health = maxHealth;
	}
}

int32_t Tank::getHealth()
{
	return health;
}

int32_t Tank::getMaxHealth()
{
	return maxHealth;
}

void Tank::setDeathAction(std::function<void()> onDeath)
{
	this->onDeath = onDeath;
}
