#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Scene.h"
#include "Physics.h"
#include "StaticBody.h"
#include "PowerUp.h"

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
	Lift(Lift&& other) noexcept;
	Lift& operator=(const Lift&) = delete;
	Lift& operator=(Lift&& other) noexcept;

	~Lift();

	void update(float t);

private:
	SceneNode* pNode;
	StaticBody* pBody;
	bool isOdd;
	DirectX::XMFLOAT3 basePosition;
};

struct Node {
	uint32_t i, j, k;
	bool operator==(const Node& rhs) const;
};

class Layer
{
public:
	Layer(Graphics& gfx,  Physics& pcs, uint32_t layerIdx, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pRoot, DirectX::FXMVECTOR color);
	~Layer();

	Layer(const Layer&) = delete;
	Layer(Layer&& other) noexcept;
	Layer& operator=(const Layer&) = delete;
	Layer& operator=(Layer&& other) noexcept;

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
	void clear();

	void update(float t, float dt);

	void spawnPowerUp(Node node);

	Layer* getLayer(size_t idx);
	
	DirectX::XMVECTOR worldPos(Node n);
	Node findNearestNode(DirectX::FXMVECTOR pos);

	uint32_t getW();
	uint32_t getD();
	uint32_t getH();

	bool hasEdge(Node n1, Node n2);
	bool hasEdge(size_t idx1, size_t idx2);

	std::vector<uint8_t>* getEdges();
	std::vector<Layer>* getLayers();

	Node getRandomDrivableNode();
	
private:
	void buildGraph(uint32_t w, uint32_t h, uint32_t d);
	
	static constexpr uint8_t emptyBlockId = 0;
	static constexpr uint8_t cubeBlockId = 1;
	static constexpr uint8_t bridgeBlockId = 2;
	static constexpr uint8_t ramp1BlockId = 3;
	static constexpr uint8_t ramp2BlockId = 4;
	static constexpr uint8_t liftBlockId = 5;

	static constexpr uint32_t totalPowerUps = 2; // ændres når power-ups tilføjes
	static constexpr uint8_t powerUpHeal = 0;
	static constexpr uint8_t powerUpDamage = 1;

	static constexpr float powerUpSpawnTime = 5.0f;

private:
	std::vector<Layer> layers;

	std::vector<uint8_t> edges;

	float powerUpSpawnCooldown;

	uint32_t w;
	uint32_t d;
	uint32_t h;

};