#include "Scene.h"

using namespace DirectX;

SceneNode::SceneNode(SceneNode* pParent) :
	scaling({ 1,1,1 }),
	translation({ 0,0,0 }),
	quaternion({ 0,0,0,1 }),
	pParent(pParent)
{}

void SceneNode::draw(Graphics& gfx, XMMATRIX parentTransform)
{
	XMMATRIX thisToParent = XMMatrixAffineTransformation(
		XMLoadFloat3(&scaling),
		XMVectorSet(0, 0, 0, 0),
		XMLoadFloat4(&quaternion),
		XMLoadFloat3(&translation)
	);
	XMMATRIX thisTransform = thisToParent * parentTransform;
	mesh.draw(gfx, thisTransform);
	for (size_t i = 0; i < children.size(); i++) {
		children[i].draw(gfx, thisTransform);
	}
}

SceneNode* SceneNode::addChild()
{
	children.emplace_back(this);
	return &children.back();
}

Mesh* SceneNode::getMesh()
{
	return &mesh;
}

SceneNode* SceneNode::getChild(size_t index)
{
	return &children[index];
}

DirectX::XMMATRIX SceneNode::localToParent() const {
	return XMMatrixAffineTransformation(
		XMLoadFloat3(&scaling),
		XMVectorZero(),
		XMLoadFloat4(&quaternion),
		XMLoadFloat3(&translation)
	);
}

DirectX::XMMATRIX SceneNode::localToWorld() const {
	if (pParent) {
		return localToParent() * pParent->localToWorld();
	} else {
		return XMMatrixIdentity();
	}
}

void SceneNode::translate(DirectX::XMVECTOR delta)
{
	XMStoreFloat3(&translation, XMLoadFloat3(&translation) + delta);
}

void SceneNode::rotate(DirectX::XMVECTOR rotation)
{
	XMStoreFloat4(&quaternion, XMQuaternionMultiply(XMLoadFloat4(&quaternion), rotation));
}

void SceneNode::scale(DirectX::XMVECTOR factor)
{
	XMStoreFloat3(&scaling, XMLoadFloat3(&scaling)*factor);
}

Scene::Scene() : 
	root(nullptr) 
{}

void Scene::draw(Graphics& gfx)
{
	root.draw(gfx, XMMatrixIdentity());
}

SceneNode* Scene::getRoot()
{
	return &root;
}
