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
	Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, Interaction& interaction);
	~Tank();

	void update(float dt) override;
	void shoot(Graphics& gfx, Physics& pcs);
	
	void rotateTurret(float angle);

	void driveForwards();
	void driveBackwards();
	void turnRight();
	void turnLeft();

	DirectX::XMMATRIX bodyToWorld();
	DirectX::XMMATRIX turretToWorld();
	
private:
	static constexpr DirectX::XMFLOAT3 boxDims = { 1.36f, 0.75f, 2.0f };
	
	static constexpr size_t turretNodeIdx = 13;

	static constexpr float dragConstant = 1.0f;
	static constexpr float acc = 2.0f;

private:
	float turretAngle;

	DirectX::XMFLOAT3 forwardDir;
	DirectX::XMFLOAT3 rightDir;
	DirectX::XMFLOAT3 upDir;

	SceneNode* pNode;
	
	int32_t health;
	int32_t maxHealth;

	RigidBody* pRB;
};
