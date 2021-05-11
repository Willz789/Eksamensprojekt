#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Graphics& gfx, Physics& pcs, Level& lvl, SceneNode* pRoot, Interaction& interaction) :
	pInteraction(&interaction),
	shooting(false),
	shotPower(0.0f),
	tankDead(false),
	pMListener(nullptr)
{
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, lvl.worldPos({4, 15, 15}));
	camera.assignTank(*pTank); 

	pTank->setDeathAction([this]() -> void {
		PolyTank::get().getPlayer().tankDied();
	});
}

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
	}
	if (pInteraction->keyDown('D')) {
		pTank->turnRight(dt);
	}


	if (shooting) {
		if (!pInteraction->lMouseDown) {
			pTank->shoot(gfx, pcs, shotPower, floor(baseDamage * damageMultiplier));
			shotPower = 0.0f;
			damageMultiplier = 1.0f;
			shooting = false;
		} else {
			shotPower += 30.0f * dt;
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

Tank* Player::getTank()
{
	return pTank;
}

Camera* Player::getCamera()
{
	return &camera;
}

uint8_t Player::getActivePowerUp()
{
	return activePowerUp;
}

void Player::setDamage(float multiplier)
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
