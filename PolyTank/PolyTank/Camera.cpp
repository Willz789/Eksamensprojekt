#include "Camera.h"

#include <algorithm>

using namespace DirectX;

Camera::Camera()
{
	isAiming = false;
}

void Camera::assignTank(Tank& tank)
{
	pTank = &tank;
}

void Camera::addPitch(float pitch)
{
	this->pitch = std::clamp(this->pitch + pitch, 0.01f, pi / 2.001f);
}

void Camera::setAim()
{
	isAiming = !isAiming;
	pTank->resetTurretPitch();
}

bool Camera::getAim()
{
	return isAiming;
}

DirectX::FXMMATRIX Camera::viewMatrix() {
	XMMATRIX turretToWorld = pTank->turretToWorld();
	if (!isAiming) {
		XMVECTOR eyePos = XMVector4Transform(XMVectorSet(0.0f, 5.0f * sin(pitch), 5.0f * cos(pitch), 1.0f), turretToWorld);
		return XMMatrixLookAtRH(eyePos, XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), turretToWorld), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}
	else {
		XMVECTOR eyeDirection = -turretToWorld.r[2];
		XMVECTOR eyePos = XMVector4Transform(XMVectorSet(0.0f, 0.1f, -0.2f, 1.0f), pTank->turretToWorld());
		return XMMatrixLookToRH(eyePos, eyeDirection, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}
	
}
