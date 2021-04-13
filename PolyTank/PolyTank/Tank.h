#pragma once

#include "Util.h"
#include "Graphics.h"
#include "Scene.h"
#include "Level.h"

class Tank {
public:
	Tank() = default;

	Tank(Graphics& gfx, SceneNode* pRoot, DirectX::FXMVECTOR initPos);

	void update(Level& lvl, float dt);
	
	void rotateTurret(float angle);

	DirectX::XMMATRIX bodyToWorld();
	DirectX::XMMATRIX turretToWorld();
	
private:
	static constexpr size_t turretNodeIdx = 13;

private:
	DirectX::XMFLOAT3 position;
	float turretAngle;

	SceneNode* pNode;
	
	int32_t health;
	int32_t maxHealth;
};
