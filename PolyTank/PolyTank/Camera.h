#pragma once

#include "Util.h"
#include "Tank.h"

class Camera {
public:
	Camera() = default;

	void assignTank(Tank& tank);
	void addPitch(float pitch);

	DirectX::FXMMATRIX viewMatrix();

private:
	Tank* pTank;
	float pitch;
};

