#include "PowerUp.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"

using namespace DirectX;

PowerUp::PowerUp(Graphics& gfx, Physics& pcs, SceneNode* pRoot, FXMVECTOR pos) :
	pNode(nullptr)
{
	pBody = pcs.emplaceBody<StaticBody>(
		std::make_unique<Box>(0.2f, 0.2f, 0.2f),
		pos + XMVectorSet(0.0f, 0.2f, 0.0f, 0.0f),
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
	pNode->rotate(XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dt));
}

PowerUpDamage::PowerUpDamage(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos) :
	PowerUp(gfx, pcs, pRoot, pos)
{
	GLTF::Loader("./Models/Power-ups/Damage/Damage.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	pcs.assignCollisionHandler(pBody, [this](Body* pOther, FXMVECTOR resolution) -> void {
		RigidBody* pColliderRB = dynamic_cast<RigidBody*>(pOther);
		if (pColliderRB) {
			Tank* pTank = dynamic_cast<Tank*>(pColliderRB->owner());
			if (pTank) {
				if (pTank == PolyTank::get().getPlayer().getTank()) {
					PolyTank::get().getPlayer().setDamage(4.0f);
					PolyTank::get().deleteGameObject(this);
				}
			}
		}
	});
}

PowerUpHealth::PowerUpHealth(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR pos) :
	PowerUp(gfx, pcs, pRoot, pos)
{
	GLTF::Loader("./Models/Power-ups/Health/Health.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	pcs.assignCollisionHandler(pBody, [this](Body* pOther, FXMVECTOR resolution) -> void {
		RigidBody* pColliderRB = dynamic_cast<RigidBody*>(pOther);
		if (pColliderRB) {
			Tank* pTank = dynamic_cast<Tank*>(pColliderRB->owner());
			if (pTank) {
				if (pTank == PolyTank::get().getPlayer().getTank()) {
					PolyTank::get().getPlayer().getTank()->heal(40);
					PolyTank::get().deleteGameObject(this);
				}
			}
		}
	});
}
