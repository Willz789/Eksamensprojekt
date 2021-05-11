#include "PowerUp.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"

using namespace DirectX;

PowerUp::PowerUp(Graphics& gfx, Physics& pcs, SceneNode* pRoot, FXMVECTOR pos)
{
	pBody = pcs.emplaceBody<StaticBody>(
		std::make_unique<Box>(0.2f, 0.2f, 0.2f),
		pos, 
		XMQuaternionIdentity()
	);
}

PowerUp::~PowerUp()
{
	if (pBody) {
		PolyTank::get().getPcs().deleteBody(pBody);
	}

	if (pNode) {
		pNode->deleteNode();
	}
}

void PowerUp::update(float dt)
{
	pNode->rotate(XMVectorSet(0.0f, dt, 0.0f, 0.0f));
}

PowerUpDamage::PowerUpDamage(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos) :
	PowerUp(gfx, pcs, pRoot, pos)
{
	GLTF::Loader("./Models/Power-ups/Damage/Damage.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();
}

PowerUpHealth::PowerUpHealth(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos) :
	PowerUp(gfx, pcs, pRoot, pos)
{
	GLTF::Loader("./Models/Power-ups/Health/Health.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();
}
