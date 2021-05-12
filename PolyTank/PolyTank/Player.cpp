#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Interaction& interaction) :
	pInteraction(&interaction),
	shooting(false),
	shotPower(0.0f),
	maxShotPower(25.0f),
	damageMultiplier(1.0f),
	points(0),
	tankDead(false),
	pMListener(nullptr)
{}

Player::~Player()
{
	if (pMListener) {
		pInteraction->removeListener(pMListener);
	}
}

void Player::update(Graphics& gfx, Physics& pcs, float dt)
{
	if (pInteraction->keyDown('W')) {
		pTank->driveForward(dt);
	}
	if (pInteraction->keyDown('S')) {
		pTank->driveBackward(dt);
	}
	if (pInteraction->keyDown('A')) {
		pTank->turnLeft(dt);
		pTank->rotateTurret(-pTank->getRotationSpeed() * dt, 0);
	}
	if (pInteraction->keyDown('D')) {
		pTank->turnRight(dt);
		pTank->rotateTurret(pTank->getRotationSpeed() * dt, 0);
	}


	if (shooting) {
		if (!pInteraction->lMouseDown) {
			pTank->shoot(gfx, pcs, shotPower, floor(baseDamage * damageMultiplier));
			shotPower = 5.0f;
			damageMultiplier = 1.0f;
			shooting = false;
		} else {
			shotPower += 25.0f * dt;
			shotPower = std::min(shotPower, maxShotPower);
		}
	}
}

void Player::initListeners(Graphics& gfx, Physics& pcs)
{
	pMListener = pInteraction->addListener([this, &gfx, &pcs](const MouseEvent& e) -> void {
		if (e.button == MouseEvent::Button::LEFT) {
			shooting = true;
			shotPower = 0.0f;
		}
		if (e.button == MouseEvent::Button::RIGHT) {
			camera.setAim();
		}

		if (e.button != MouseEvent::Button::MOVE) return;

		float deltaYaw = -0.004f * e.mousex;
		float deltaPitch = 0.004f * e.mousey;

		if (camera.getAim()) {
			pTank->rotateTurret(deltaYaw, -deltaPitch);
		}
		else {
			pTank->rotateTurret(deltaYaw, 0);
			camera.addPitch(deltaPitch);
		}
	});
}

void Player::removeListeners()
{
	if (pMListener) {
		pInteraction->removeListener(pMListener);
	}
}

void Player::generateNewTank(Graphics& gfx, Physics& pcs, Level& lvl, SceneNode* pRoot)
{
	shooting = false;
	damageMultiplier = 1;
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, lvl.worldPos(lvl.getRandomPathableNode()));
	camera.assignTank(*pTank);
	pTank->setDeathAction([this]() -> void {
		PolyTank::get().getPlayer().tankDied();
	});
}

Tank* Player::getTank()
{
	return pTank;
}

Camera* Player::getCamera()
{
	return &camera;
}

void Player::setDamage(uint8_t multiplier)
{
	damageMultiplier += multiplier;
}

void Player::tankDied()
{
	tankDead = true;
}

bool Player::isTankDead()
{
	return tankDead;
}

float Player::getShotPower()
{
	return shotPower;
}

float Player::getMaxShotPower()
{
	return maxShotPower;
}

uint8_t Player::getDamageMultiplier()
{
	return damageMultiplier;
}

uint32_t Player::getPoints()
{
	return points;
}

void Player::addPoints(uint32_t addedPoints)
{
	points += addedPoints;
}