#include "Player.h"
#include "PolyTank.h"

using namespace DirectX;

Player::Player(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Interaction& interaction) :
	pInteraction(pInteraction)
{
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

	pKListener = pInteraction->addListener([this](const KeyEvent& e) -> void {
		if (e.key == 'w') {
			pTank->driveForwards();
		}
		if (e.key == 's') {
			pTank->driveBackwards();
		}
		if (e.key == 'a') {
			pTank->turnLeft();
		}
		if (e.key == 'd') {
			pTank->turnRight();
		}
	});
}

Camera* Player::getCamera()
{
	return &camera;
}
