#pragma once

#include <vector>
#include "Graphics.h"
#include "Mesh.h"



class SceneNode
{
public:
	SceneNode(SceneNode* pParent);

	void draw(Graphics& gfx, DirectX::XMMATRIX parentTransform);

	SceneNode* addChild();

	Mesh* getMesh();
	SceneNode* getChild(size_t index);

	DirectX::XMMATRIX localToParent() const;
	DirectX::XMMATRIX localToWorld() const;

	void translate(DirectX::XMVECTOR delta);
	void rotate(DirectX::XMVECTOR rotation);
	void scale(DirectX::XMVECTOR factor);

private:
	SceneNode* pParent;
	std::vector<SceneNode> children;
	Mesh mesh;

	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 scaling;
};

class Scene
{
public:
	Scene();

	void draw(Graphics& gfx);
	SceneNode* getRoot();

private:
	SceneNode root;
};

