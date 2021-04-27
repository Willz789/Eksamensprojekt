#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction) :
	pInteraction(&interaction)
{
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, XMVectorSet(0.0f, 4.0f, 0.0f, 0.0f));
	camera.assignTank(*pTank);
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
			pTank->shoot(gfx, pcs, shotPower);
			shotPower = 0.0f;
			shooting = false;
		} 		else {
			shotPower += 10.0f * dt;
		}
	}
}

void Player::initListeners(Graphics& gfx, Physics& pcs)
{
	pMListener = pInteraction->addListener([this, &gfx, &pcs](const MouseEvent& e) -> void {
		if (e.button == MouseEvent::Button::LEFT) {
			shooting = true;
			shotPower = 0;
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

Camera* Player::getCamera()
{
	return &camera;
}
