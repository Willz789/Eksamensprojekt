#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction) :
	pInteraction(&interaction),
	pMListener(nullptr)
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

void Player::initListeners(Graphics& gfx, Physics& pcs)
{
	pMListener = pInteraction->addListener([this, &gfx, &pcs](const MouseEvent& e) -> void {
		if (e.button == MouseEvent::Button::LEFT) {
			pTank->shoot(gfx, pcs);
		}

		if (e.button != MouseEvent::Button::MOVE) return;

		float deltaYaw = -0.004f * e.mousex;
		float deltaPitch = 0.004f * e.mousey;

		pTank->rotateTurret(deltaYaw);
		camera.addPitch(deltaPitch);
	});
}

void Player::update() {
	if (pInteraction->keyDown('W')) {
		pTank->driveForward();
	}
	if (pInteraction->keyDown('S')) {
		pTank->driveBackward();
	}
	if (pInteraction->keyDown('A')) {
		pTank->turnLeft();
	}
	if (pInteraction->keyDown('D')) {
		pTank->turnRight();
	}
}

Camera* Player::getCamera()
{
	return &camera;
}
