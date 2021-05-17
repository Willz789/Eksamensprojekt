#pragma once

#include "Util.h"
#include "Tank.h"

class Camera {
public:
	Camera();

	void assignTank(Tank& tank);
	void addPitch(float pitch);

	void setAim();
	bool getAim();

	DirectX::FXMMATRIX viewMatrix();

private:
	bool isAiming;

	Tank* pTank;
	float pitch;
};

