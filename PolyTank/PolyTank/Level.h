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

class Layer
{
public:
	Layer(Graphics& gfx,  Physics& pcs, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pNode, DirectX::FXMVECTOR color);

	SceneNode* getNode();

	void moveUp(uint32_t n);
	bool doesBlockExist(uint32_t i, uint32_t j) const;
	bool isBlockSolid (uint32_t i, uint32_t j) const;

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

	uint32_t w;
	uint32_t d;

	Mesh* pMesh;
	SceneNode* pNode;
};

class Level
{
public:
	Level() = default;
	Level(Graphics& gfx, Physics& pcs, const std::filesystem::path& file, Scene& scene);

	Layer* getLayer(size_t idx);
	Layer* getLayer(DirectX::FXMVECTOR position);
	

private:
	std::vector<Layer> layers;

	std::vector<uint8_t> edges;

};