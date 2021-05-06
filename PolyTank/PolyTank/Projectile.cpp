#include "Projectile.h"

#include "GLTFLoader.h"
#include "Box.h"
#include "PolyTank.h"

using namespace DirectX;

Projectile::Projectile(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot, float initVel)
{
	GLTF::Loader("./Models/projectile/projectile2.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	pRB = pcs.emplaceBody<RigidBody>(
		std::make_unique<Box>(0.04f, 0.04f, 0.04f),
		initPos,
		initRot,
		0.05f
	);

	pcs.assignCollisionHandler(pRB, [this](Body* pOther, FXMVECTOR resolution) -> void {
		if (dynamic_cast<StaticBody*>(pOther)) {
			PolyTank::get().deleteGameObject(this);
		}
	});

	XMVector4Transform(XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), XMMatrixRotationQuaternion(initRot));

	pRB->addMoment(XMVector4Transform(XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), XMMatrixRotationQuaternion(initRot))*initVel*pRB->getMass());
}

Projectile::~Projectile()
{
	PolyTank::get().getPcs().deleteBody(pRB);
	pNode->deleteNode();
}

void Projectile::update(float dt)
{
	pNode->setTranslation(pRB->getPosition());

	if (XMVectorGetY(pRB->getPosition()) < -1) {
		PolyTank::get().deleteGameObject(this);
	}
}
