#pragma once

#include "Util.h"
#include "Graphics.h"
#include "Scene.h"
#include "Physics.h"
#include "RigidBody.h"
#include "Projectile.h"
#include "Interaction.h"
#include "IGameObject.h"

class Tank : public IGameObject {

public:
	Tank() = default;
	Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos);

	~Tank();

	void update(float dt) override;
	void shoot(Graphics& gfx, Physics& pcs, float power);
	
	void rotateTurret(float yaw, float pitch);
	void setTurretRotation(float yaw, float pitch);
	void resetTurretPitch();

	void driveForward(float dt);
	void driveBackward(float dt);
	void turnRight(float dt);
	void turnLeft(float dt);

	DirectX::XMMATRIX bodyToWorld();
	DirectX::XMMATRIX turretToWorld();

	DirectX::XMVECTOR getTurretTipPos();
	DirectX::XMVECTOR getPosition();
	DirectX::XMVECTOR getGroundPosition();

	void takeDamage(int32_t damage);
	void die();
	int32_t getHealth();
	int32_t getMaxHealth();

	void setDeathAction(std::function<void()> onDeath);

private:
	static constexpr DirectX::XMFLOAT3 boxDims = { 0.68f, 0.4f, 1.0f };
	
	static constexpr size_t turretNodeIdx = 13;

	static constexpr float dragConstant = 10.0f;
	static constexpr float acc = 8.0f;

private:
	float turretYaw;
	float turretPitch;

	DirectX::XMFLOAT3 forwardDir;
	DirectX::XMFLOAT3 rightDir;
	DirectX::XMFLOAT3 upDir;
	
	DirectX::XMFLOAT4 turretTipPos;
	
	int32_t health;
	int32_t maxHealth;
	std::function<void()> onDeath;

	SceneNode* pNode;
	RigidBody* pRB;

};
