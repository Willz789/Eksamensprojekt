#include "Projectile.h"

#include "GLTFLoader.h"

using namespace DirectX;

Projectile::Projectile(Graphics& gfx, Physics& pcs, SceneNode* pRoot, DirectX::FXMVECTOR initPos, DirectX::FXMVECTOR initRot, float initVel)
{
	GLTF::Loader("./Models/projectile/projectile.gltf").getScene(gfx, pRoot);
	pNode = pRoot->lastChild();

	pRB = pcs.addBody(std::make_unique<RigidBody>(
		std::make_unique<Box>(1.36f, 0.75f, 2.0f),
		0.05f,
		initPos,
		initRot
	));

	XMVector4Transform(XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), XMMatrixRotationQuaternion(initRot));

	pRB->addMoment(XMVector4Transform(XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), XMMatrixRotationQuaternion(initRot))*initVel*pRB->getMass());
}

void Projectile::update(float dt)
{
	pRB->setRotation(XMQuaternionRotationMatrix(XMMatrixLookToRH(pRB->getPosition(), pRB->getLinMoment(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))));
	pNode->setTranslation(pRB->getPosition() - XMVectorSet(0.0f, 0.75f / 2.0f, 0.0f, 0.0f));
	pNode->setRotation(pRB->getRotation());
}