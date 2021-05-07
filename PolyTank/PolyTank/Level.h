#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Scene.h"
#include "Physics.h"
#include "StaticBody.h"

#include <filesystem>
#include <unordered_map>

struct Block
{
	std::vector<Index> indices;
	std::vector<DefaultVertex> vertices;
};

class Lift {
public:
	Lift(Graphics& gfx, Physics& pcs, SceneNode* pLayerNode, const Block& b, uint32_t layerIdx, uint32_t i, uint32_t j, uint32_t d, uint32_t w);
	
	Lift(const Lift&) = delete;
	Lift(Lift&& other);
	Lift& operator=(const Lift&) = delete;
	Lift& operator=(Lift&& other);

	~Lift();

	void update(float t);

private:
	SceneNode* pNode;
	StaticBody* pBody;

	DirectX::XMFLOAT3 basePosition;
};

struct Node {
	uint32_t i, j, k;
	bool visited;
	float shortestDist;
	std::vector<Node*> pathToNode;
	bool operator==(const Node& rhs) const;
};

class Layer
{
public:
	Layer(Graphics& gfx,  Physics& pcs, uint32_t layerIdx, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pNode, DirectX::FXMVECTOR color);
	~Layer();

	Layer(const Layer&) = delete;
	Layer(Layer&& other) = default;
	Layer& operator=(const Layer&) = delete;
	Layer& operator=(Layer&& other) = default;

	SceneNode* getNode();

	bool doesBlockExist(uint32_t i, uint32_t j) const;
	bool isBlockSolid (uint32_t i, uint32_t j) const;
	
	void update(float t, float dt);

	std::vector<StaticBody*>* getBlockBodies();

	uint8_t getBlockType(uint32_t i, uint32_t j);

private:
	static Block getBlock(uint8_t id);

	static constexpr uint8_t emptyBlockId = 0;
	static constexpr uint8_t cubeBlockId = 1;
	static constexpr uint8_t bridgeBlockId = 2;
	static constexpr uint8_t ramp1BlockId = 3;
	static constexpr uint8_t ramp2BlockId = 4;
	static constexpr uint8_t liftBlockId = 5;


private:
	std::vector<uint8_t> blocks;
	std::vector<StaticBody*> blockBodies;
	std::vector<Lift> lifts;

	uint32_t w;
	uint32_t d;

	Mesh* pMesh;
	SceneNode* pNode;
};

class Level
{
public:
	Level() = default;

	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;

	void loadFile(Graphics& gfx, Physics& pcs, const std::filesystem::path& file, Scene& scene);
	
	void update(float t, float dt);

	Layer* getLayer(size_t idx);
	Layer* getLayer(DirectX::FXMVECTOR position);

	Node findNearestNode(DirectX::FXMVECTOR pos);

	uint32_t getW();
	uint32_t getD();
	uint32_t getH();

	std::vector<uint8_t>* getEdges();
	std::vector<Layer>* getLayers();
	
private:
	void buildGraph(uint32_t w, uint32_t h, uint32_t d);
	
	static constexpr uint8_t emptyBlockId = 0;
	static constexpr uint8_t cubeBlockId = 1;
	static constexpr uint8_t bridgeBlockId = 2;
	static constexpr uint8_t ramp1BlockId = 3;
	static constexpr uint8_t ramp2BlockId = 4;
	static constexpr uint8_t liftBlockId = 5;

private:
	std::vector<Layer> layers;

	std::vector<uint8_t> edges;

	uint32_t w;
	uint32_t d;
	uint32_t h;

};