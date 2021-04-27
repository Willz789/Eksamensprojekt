#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction) :
	pInteraction(&interaction)
{
	//pInteraction = &interaction;
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, XMVectorSet(0.0f, 4.0f, 0.0f, 0.0f));
	camera.assignTank(*pTank);
}

Player::~Player()
{
	if (pInteraction) {
		if (pMListener) {
			pInteraction->removeListener(pMListener);
		}
		if (pKListener) {
			pInteraction->removeListener(pKListener);
		}
	}
}

void Player::update(Graphics& gfx, Physics& pcs, float dt)
{
	if (shooting) {
		if (!pInteraction->lMouseDown) {
			pTank->shoot(gfx, pcs, shotPower);
			shotPower = 0;
			shooting = false;
		}
		else {
			shotPower += 0.05;
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

	pKListener = pInteraction->addListener([this](const KeyEvent& e) -> void {
		if (e.key == 'W') {
			pTank->driveForwards();
		}
		if (e.key == 'S') {
			pTank->driveBackwards();
		}
		if (e.key == 'A') {
			pTank->turnLeft();
		}
		if (e.key == 'D') {
			pTank->turnRight();
		}
	});
}

Camera* Player::getCamera()
{
	return &camera;
}
