#pragma once

#include "Util.h"
#include "Graphics.h"
#include "Scene.h"
#include "Physics.h"
#include "RigidBody.h"
#include "Projectile.h"
#include "Interaction.h"

class Tank {
public:
	Tank() = default;
	Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, Interaction& interaction, Tank* pDest);

	void update(float dt);
	void shoot(Graphics& gfx, Physics& pcs);
	
	void rotateTurret(float angle);

	DirectX::XMMATRIX bodyToWorld();
	DirectX::XMMATRIX turretToWorld();
	
private:
	static constexpr DirectX::XMFLOAT3 hitboxDims = { 1.36f, 0.75f, 2.0f };
	
	static constexpr size_t turretNodeIdx = 13;

private:
	float turretAngle;

	SceneNode* pNode;
	
	int32_t health;
	int32_t maxHealth;

	RigidBody* pRB;

	Projectile proj;
	bool projExist;
};
