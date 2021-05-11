#pragma once

#include "Util.h"
#include "IGameObject.h"
#include "StaticBody.h"
#include "Scene.h"
#include "Physics.h"

class PowerUp : public IGameObject {
public:
	PowerUp(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos);
	~PowerUp();

	void update(float dt) override;
	
protected:
	StaticBody* pBody;
	SceneNode* pNode;
};

class PowerUpDamage : public PowerUp {
public:
	PowerUpDamage(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos);
};

class PowerUpHealth : public PowerUp {
public:
	PowerUpHealth(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos);
};