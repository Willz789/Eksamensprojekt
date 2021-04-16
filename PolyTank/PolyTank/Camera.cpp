#include "Camera.h"

#include <algorithm>

using namespace DirectX;

Camera::Camera(Interaction* pInteraction, Tank& tank) :
	pitch(0.0f),
	pTank(&tank),
	pInteraction(pInteraction) {

	pListener = pInteraction->addListener([this](const MouseEvent& e) -> void {
		if (e.button != MouseEvent::Button::MOVE) return;

		float deltaYaw = -0.004f * e.mousex;
		float deltaPitch = 0.004f * e.mousey;

		pTank->rotateTurret(deltaYaw);
		pitch = std::clamp(pitch + deltaPitch, -pi / 8.0f, pi / 2.001f);
	});
}

Camera::~Camera() {
	if (pListener) {
		pInteraction->removeListener(pListener);
	}
}

DirectX::FXMMATRIX Camera::viewMatrix() {
	XMMATRIX turretToWorld = pTank->turretToWorld();
	XMVECTOR eyePos = XMVector4Transform(XMVectorSet(0.0f, 5.0f * sin(pitch), 5.0f * cos(pitch), 1.0f), turretToWorld);

	return XMMatrixLookAtRH(eyePos, XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), turretToWorld), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}
