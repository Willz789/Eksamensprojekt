#include "RigidBody.h"
#include "Physics.h"

using namespace DirectX;

RigidBody::RigidBody(std::unique_ptr<ConvexShape>&& pShape, float m, FXMVECTOR initPos, FXMVECTOR initRot) :
	pShape(std::move(pShape)),
	mass(m)
{
	XMStoreFloat3(&position, initPos);
	XMStoreFloat4(&rotation, initRot);
	XMStoreFloat3(&linMom, { 0.0f, 0.0f, 0.0f });
	XMStoreFloat3(&angMom, { 0.0f, 0.0f, 0.0f });
}

void RigidBody::addForce(FXMVECTOR force)
{
	XMVECTOR extForces = XMLoadFloat3(&externalForces);
	extForces += force;
}

void RigidBody::addForce(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point)
{
	XMVECTOR extForces = XMLoadFloat3(&externalForces);
	extForces += force;

	XMVECTOR extTorque = XMLoadFloat3(&externalTorques);
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR pointRelative = point - pos;
	extTorque += XMVector3Cross(pointRelative, force);
}

void RigidBody::addMoment(DirectX::FXMVECTOR moment)
{
	XMVECTOR linMoment = XMLoadFloat3(&linMom);
	linMoment += moment;
}

void RigidBody::addAngMoment(DirectX::FXMVECTOR moment)
{
	XMVECTOR angMoment = XMLoadFloat3(&angMom);
	angMoment += moment;
}

inline void applyForce(XMVECTOR& linMoment, XMVECTOR& pos, FXMVECTOR force, float dt, float mass) 
{
	linMoment += force * dt;
	XMVECTOR velocity = linMoment * 1/mass;
	pos += velocity * dt;
}

inline void applyTorque(XMVECTOR& angMoment, XMVECTOR& rot, FXMVECTOR torque, float dt, FXMMATRIX invInertiaBody) 
{
	angMoment += torque * dt;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rot);
	XMMATRIX invInertia = XMMatrixTranspose(rotation) * invInertiaBody * rotation;
	XMVECTOR angVelocity = XMVector3Transform(angMoment, invInertia);
	rot += 0.5f * XMQuaternionMultiply(rot, angVelocity) * dt;
}

void RigidBody::update(float dt)
{
	XMVECTOR linMoment = XMLoadFloat3(&linMom);
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR angMoment = XMLoadFloat3(&angMom);
	XMVECTOR quaternion = XMLoadFloat4(&rotation);
	XMMATRIX invI = XMLoadFloat3x3(&invInertia);

	XMVECTOR forces = XMLoadFloat3(&externalForces);
	XMVECTOR torques = XMLoadFloat3(&externalTorques);
	applyForce(linMoment, pos, forces, dt, mass);
	applyTorque(angMoment, quaternion, torques, dt, invI);

	externalForces = { 0.0f, 0.0f, 0.0f};
	externalTorques = {0.0f, 0.0f, 0.0f};

	XMStoreFloat3(&linMom, linMoment);
	XMStoreFloat3(&position, pos);
}



