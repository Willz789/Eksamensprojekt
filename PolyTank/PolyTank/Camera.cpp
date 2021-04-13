#include "Camera.h"

#include <iostream>

using namespace DirectX;

Camera::Camera(Interaction* pInteraction, Tank& tank) :
	pitch(0.0f),
	pTank(&tank),
	pInteraction(pInteraction) {

	pListener = pInteraction->addListener([this](const MouseEvent& e) -> void {
		if (e.button != MouseEvent::Button::MOVE) return;


		static uint32_t oldMX = e.mousex, oldMY = e.mousey;
		int32_t deltaX = e.mousex - oldMX;
		int32_t deltaY = e.mousey - oldMY;
		oldMX = e.mousex;
		oldMY = e.mousey;

		float deltaYaw = -0.005f * deltaX;
		float deltaPitch = 0.005f * deltaY;

		pTank->rotateTurret(deltaYaw);
		pitch += deltaPitch;
	});
}

Camera::~Camera() {
	if (pListener) {
		pInteraction->removeListener(pListener);
	}
}

DirectX::FXMMATRIX Camera::viewMatrix() {
	XMMATRIX turretToWorld = pTank->turretToWorld();
	XMVECTOR eyePos = XMVector4Transform(4.0f * XMVectorSet(0.0f, sin(pitch), cos(pitch), 0.0f), turretToWorld);

	return XMMatrixLookAtRH(eyePos, XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), turretToWorld), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}
