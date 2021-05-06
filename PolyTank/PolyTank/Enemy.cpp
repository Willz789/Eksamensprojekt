#include "Enemy.h"
#include "PolyTank.h"

#include <random>

using namespace DirectX;

Enemy::Enemy(Graphics& gfx, Physics& pcs, SceneNode* pRoot, Tank* pTarget, Level& lvl) :
	pTarget(pTarget)
{
	pTank = PolyTank::get().emplaceGameObject<Tank>(gfx, pcs, pRoot, XMVectorSet(10.0f, 4.0f, 10.0f, 0.0f));
	pLvl = &lvl;
}

void Enemy::update(float dt)
{
	
}

void Enemy::buildPath() 
{
	static std::random_device rd;
	static std::default_random_engine rng(rd());
	std::uniform_real_distribution uDist(0.0f, 2.0f*pi);
	float angle = uDist(rng);
	XMVECTOR targetOffset = 4.0f * XMVectorSet(cos(angle), 0.0f, sin(angle), 0.0f);
	
	Node targetNode;
	XMVECTOR targetPos = pTarget->getPosition() + targetOffset;

	targetNode.i = uint32_t(XMVectorGetY(targetPos));
	targetNode.j = uint32_t(XMVectorGetZ(targetPos));
	targetNode.k = uint32_t(XMVectorGetX(targetPos));
	
	Node currentNode;
	XMVECTOR currentPos = pTank->getPosition();
	currentNode.i = uint32_t(XMVectorGetY(currentPos));
	currentNode.j = uint32_t(XMVectorGetZ(currentPos));
	currentNode.k = uint32_t(XMVectorGetX(currentPos));

}