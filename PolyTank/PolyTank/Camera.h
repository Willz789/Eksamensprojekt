#pragma once

#include "Util.h"
#include "Tank.h"

class Camera {
public:
	Camera() = default;
	Camera(Interaction* pInteraction, Tank& tank);

	Camera(const Camera&) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&& other) = delete;

	~Camera();

	DirectX::FXMMATRIX viewMatrix();

private:
	Tank* pTank;
	float pitch;


	MouseListener* pListener;
	Interaction* pInteraction;
};

