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
	for (SceneNode& child : children) {
		child.draw(gfx, thisTransform);
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
	auto it = children.begin();
	for (size_t i = 0; i < index; i++) {
		it++;
	}
	return &*it;
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

void SceneNode::setTranslation(DirectX::XMVECTOR delta) {
	XMStoreFloat3(&translation, delta);
}

void SceneNode::setRotation(DirectX::XMVECTOR rotation) {
	XMStoreFloat4(&quaternion, rotation);
}

void SceneNode::setScaling(DirectX::XMVECTOR factor) {
	XMStoreFloat3(&scaling, factor);
}

void SceneNode::reset()
{
	clear();
	translation = { 0,0,0 };
	quaternion = { 0,0,0,1 };
	scaling = { 1,1,1 };
}

void SceneNode::clear()
{
	children.clear();
	mesh.reset();
}

Scene::Scene(Graphics& gfx) : 
	root(nullptr),
	lightingCBuf(gfx, 0, sizeof(Lighting)) {
	
	XMStoreFloat3(&lighting.sun.direction, XMVector3Normalize(XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f)));
	XMStoreFloat3(&lighting.sun.color, XMVectorSet(3.0f, 3.0f, 3.0f, 0.0f));
	XMStoreFloat3(&lighting.ambient.color, XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));

}

void Scene::draw(Graphics& gfx)
{
	Lighting viewspaceLighting = lighting;
	XMStoreFloat3(&viewspaceLighting.sun.direction, XMVector4Transform(XMLoadFloat3(&lighting.sun.direction), gfx.getCamera()));

	lightingCBuf.update(gfx, viewspaceLighting);
	lightingCBuf.bind(gfx);

	root.draw(gfx, XMMatrixIdentity());
}

SceneNode* Scene::getRoot()
{
	return &root;
}
