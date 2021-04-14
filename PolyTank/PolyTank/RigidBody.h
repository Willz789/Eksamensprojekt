#pragma once

#include "ConvexShape.h"
#include "Util.h"

#include <memory>

class RigidBody {
public:
	RigidBody(std::unique_ptr<ConvexShape>&& pShape, float m, FXMVECTOR initPos, FXMVECTOR initRot);

	void addForce(DirectX::FXMVECTOR force);
	void addForce(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point);
	void addMoment(DirectX::FXMVECTOR moment);
	void addAngMoment(DirectX::FXMVECTOR angMoment);
	void update(float dt);

public:
	const float mass;

private:
	std::unique_ptr<ConvexShape> pShape;
	
	DirectX::XMFLOAT3X3 invInertia;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 linMom;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 angMom;

	DirectX::XMFLOAT3 externalForces;
	DirectX::XMFLOAT3 externalTorques;
};
