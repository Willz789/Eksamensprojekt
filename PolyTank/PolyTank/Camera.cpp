#include "Camera.h"

#include <algorithm>

using namespace DirectX;

void Camera::assignTank(Tank& tank)
{
	pTank = &tank;
}

void Camera::addPitch(float pitch)
{
	this->pitch = std::clamp(this->pitch + pitch, -pi / 8.0f, pi / 2.001f);
}

DirectX::FXMMATRIX Camera::viewMatrix() {
	XMMATRIX turretToWorld = pTank->turretToWorld();
	XMVECTOR eyePos = XMVector4Transform(XMVectorSet(0.0f, 5.0f * sin(pitch), 5.0f * cos(pitch), 1.0f), turretToWorld);
	
	return XMMatrixLookAtRH(eyePos, XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), turretToWorld), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}
