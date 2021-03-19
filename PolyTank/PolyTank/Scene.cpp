#include "Scene.h"

using namespace DirectX;

SceneNode::SceneNode() :
	scaling({ 1,1,1 }),
	translation({ 0,0,0 }),
	quaternion({ 0,0,0,1 })
{}

void SceneNode::draw(Graphics& gfx, XMMATRIX parentTransform)
{
	XMMATRIX thisToParent = XMMatrixAffineTransformation(
		XMLoadFloat3(&scaling),
		XMVectorSet(0, 0, 0, 0),
		XMLoadFloat4(&quaternion),
		XMLoadFloat3(&translation)
	);
	XMMATRIX thisTransform = thisToParent*parentTransform;
	mesh.draw(gfx, thisTransform);
	for (size_t i = 0; i < children.size(); i++) {
		children[i].draw(gfx, thisTransform);
	}
}

SceneNode* SceneNode::addChild()
{
	children.emplace_back();
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

void Scene::draw(Graphics& gfx)
{
	root.draw(gfx, XMMatrixIdentity());
}

SceneNode* Scene::getRoot()
{
	return &root;
}
