#pragma once

#include "Graphics.h"
#include "Physics.h"
#include "Scene.h"

class Projectile
{
public:
	Projectile() = default;
	Projectile(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot, float initVel);

	void update(float dt);

private:
	SceneNode* pNode;
	RigidBody* pRB;
};

