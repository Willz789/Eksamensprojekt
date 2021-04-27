#include "RigidBody.h"
#include "Physics.h"

#include <utility>

using namespace DirectX;

RigidBody::RigidBody(std::unique_ptr<ConvexShape>&& pShape, FXMVECTOR initPos, FXMVECTOR initRot, float m) :
	Body(std::move(pShape), initPos, initRot),
	mass(m)
{

	XMMATRIX inertia = mass * this->pShape->inertiaTensor();
	inertia.r[3] = XMVectorSetW(inertia.r[3], 1.0f);

	XMStoreFloat3x3(&invInertia, XMMatrixInverse(nullptr, inertia));

	linMom = { 0.0f, 0.0f, 0.0f };
	angMom = { 0.0f, 0.0f, 0.0f };
	externalForces = { 0.0f, 0.0f, 0.0f };
	externalTorques = { 0.0f, 0.0f, 0.0f };

}

DirectX::XMVECTOR RigidBody::getLinMoment() const
{
	return XMLoadFloat3(&linMom);
}

DirectX::XMVECTOR RigidBody::getAngMoment() const {
	return XMLoadFloat3(&angMom);
}

float RigidBody::getMass() const
{
	return mass;
}

float RigidBody::getInvMass() const 
{
	return 1.0f / mass;
}

XMMATRIX RigidBody::getInvInertia() const
{
	return XMLoadFloat3x3(&invInertiaWorld);
}

void RigidBody::addForce(FXMVECTOR force)
{
	XMVECTOR extForces = XMLoadFloat3(&externalForces);
	extForces += force;

	XMStoreFloat3(&externalForces, extForces);
}

void RigidBody::addForce(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point)
{
	XMVECTOR extForces = XMLoadFloat3(&externalForces);
	extForces += force;

	XMVECTOR extTorque = XMLoadFloat3(&externalTorques);
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR pointRelative = point - pos;
	extTorque += XMVector3Cross(pointRelative, force);

	XMStoreFloat3(&externalForces, extForces);
	XMStoreFloat3(&externalTorques, extTorque);
}

void RigidBody::addTorque(DirectX::FXMVECTOR torque)
{
	XMVECTOR extTorques = XMLoadFloat3(&externalTorques);
	extTorques += torque;

	XMStoreFloat3(&externalTorques, extTorques);
}

void RigidBody::addMoment(DirectX::FXMVECTOR moment)
{
	XMVECTOR linMoment = XMLoadFloat3(&linMom);
	linMoment += moment;

	XMStoreFloat3(&linMom, linMoment);
}

void RigidBody::addAngMoment(DirectX::FXMVECTOR moment)
{
	XMVECTOR angMoment = XMLoadFloat3(&angMom);
	angMoment += moment;

	XMStoreFloat3(&angMom, angMoment);
}

DirectX::XMVECTOR RigidBody::getForce() {
	return XMLoadFloat3(&externalForces);
}

inline void applyForce(XMVECTOR& linMoment, XMVECTOR& pos, FXMVECTOR force, float dt, float mass) 
{
	linMoment += force * dt;
	XMVECTOR velocity = linMoment * 1/mass;
	pos += velocity * dt;
}

inline void applyTorque(XMVECTOR& angMoment, XMVECTOR& rot, FXMVECTOR torque, float dt, FXMMATRIX invInertiaBody, XMFLOAT3X3* invInertiaWorld) 
{
	angMoment += torque * dt;
	XMMATRIX rotation = XMMatrixRotationQuaternion(rot);
	XMMATRIX invInertia = XMMatrixTranspose(rotation) * invInertiaBody * rotation;
	XMVECTOR angVelocity = XMVector3Transform(angMoment, invInertia);

	rot += 0.5f * XMQuaternionMultiply(rot, XMVectorSetW(angVelocity, 0.0f)) * dt;
	rot = XMQuaternionNormalize(rot);

	XMStoreFloat3x3(invInertiaWorld, invInertia);
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
	applyTorque(angMoment, quaternion, torques, dt, invI, &invInertiaWorld);

	externalForces = { 0.0f, 0.0f, 0.0f};
	externalTorques = {0.0f, 0.0f, 0.0f};

	XMStoreFloat3(&linMom, linMoment);
	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&angMom, angMoment);
	XMStoreFloat4(&rotation, quaternion);
}
