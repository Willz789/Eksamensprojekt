#pragma once

#include "Graphics.h"
#include "Physics.h"
#include "Scene.h"
#include "IGameObject.h"
#include "RigidBody.h"


class Projectile : public IGameObject 
{
public:
	Projectile() = default;
	Projectile(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot, float initVel, int32_t damage);
	~Projectile();

	void update(float dt) override;

private:
	SceneNode* pNode;
	RigidBody* pRB;
};

