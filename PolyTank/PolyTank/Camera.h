#pragma once

#include "Util.h"
#include "Tank.h"

class Camera {
public:
	Camera() = default;
	Camera(Interaction* pInteraction);

	Camera(const Camera&) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&& other) = delete;

	~Camera();

	void assignTank(Tank& tank);

	DirectX::FXMMATRIX viewMatrix();

private:
	Tank* pTank;
	float pitch;


	MouseListener* pListener;
	Interaction* pInteraction;
};

