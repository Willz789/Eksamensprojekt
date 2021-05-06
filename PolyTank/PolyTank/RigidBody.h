#pragma once

#include "Body.h"
#include "Util.h"

#include <memory>

class RigidBody : public Body {
public:
	RigidBody(std::unique_ptr<ConvexShape>&& pShape, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot, float m);

	DirectX::XMVECTOR getLinMoment() const;
	DirectX::XMVECTOR getAngMoment() const;
	float getMass() const;
	float getInvMass() const override;
	DirectX::XMMATRIX getInvInertia() const;

	void addForce(DirectX::FXMVECTOR force);
	void addForce(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point);
	void addTorque(DirectX::FXMVECTOR torque);
	void addMoment(DirectX::FXMVECTOR moment);
	void addAngMoment(DirectX::FXMVECTOR angMoment);
	void rotateLinMoment(DirectX::FXMVECTOR rot);

	DirectX::XMVECTOR getForce();

	void update(float dt);

private:
	float mass;

	DirectX::XMFLOAT3X3 invInertia;
	DirectX::XMFLOAT3X3 invInertiaWorld;

	DirectX::XMFLOAT3 linMom;
	DirectX::XMFLOAT3 angMom;

	DirectX::XMFLOAT3 externalForces;
	DirectX::XMFLOAT3 externalTorques;
};
