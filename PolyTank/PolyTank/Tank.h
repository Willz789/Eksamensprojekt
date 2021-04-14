#pragma once

#include "Util.h"
#include "Graphics.h"
#include "Scene.h"
#include "Level.h"
#include "Physics.h"
#include "RigidBody.h"

class Tank {
public:
	Tank() = default;
	Tank(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos);

	void update(Level& lvl, float dt);
	
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
};
