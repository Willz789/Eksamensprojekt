#pragma once

#include <list>
#include "Graphics.h"
#include "Mesh.h"

class SceneNode
{
public:
	SceneNode(SceneNode* pParent);
	SceneNode(const SceneNode&) = delete;
	SceneNode(SceneNode&&) = default;

	SceneNode& operator=(const SceneNode&) = delete;
	SceneNode& operator=(SceneNode&&) = default;

	void draw(Graphics& gfx, DirectX::XMMATRIX parentTransform);

	SceneNode* addChild();

	Mesh* getMesh();
	SceneNode* getChild(size_t index);

	DirectX::XMMATRIX localToParent() const;
	DirectX::XMMATRIX localToWorld() const;

	void translate(DirectX::XMVECTOR delta);
	void rotate(DirectX::XMVECTOR rotation);
	void scale(DirectX::XMVECTOR factor);

	void setTranslation(DirectX::XMVECTOR delta);
	void setRotation(DirectX::XMVECTOR rotation);
	void setScaling(DirectX::XMVECTOR factor);

	void reset();
	void clear();

private:
	SceneNode* pParent;
	std::list<SceneNode> children;
	Mesh mesh;

	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 scaling;
};

struct Lighting {
	struct {
		DirectX::XMFLOAT3 direction;
		float pad0;
		DirectX::XMFLOAT3 color;
		float pad1;
	} sun;

	struct {
		DirectX::XMFLOAT3 color;
		float pad;
	} ambient;
};

class Scene
{
public:
	Scene(Graphics& gfx);

	void draw(Graphics& gfx);
	SceneNode* getRoot();
	

public:
	Lighting lighting;

private:
	SceneNode root;

	PSConstantBuffer lightingCBuf;
};

