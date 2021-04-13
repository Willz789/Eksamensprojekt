#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Scene.h"

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
	Layer(Graphics& gfx, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pNode, DirectX::FXMVECTOR color);

	SceneNode* getNode();

private:
	static Block getBlock(uint8_t id);

private:
	std::vector<uint8_t> blocks;
	uint32_t w;
	uint32_t d;

	Mesh* pMesh;
	SceneNode* pNode;
};

class Level
{
public:
	Level() = default;
	Level(Graphics& gfx, const std::filesystem::path& file, Scene& scene);

	Layer* getLayer(size_t idx);
	Layer* getLayer(DirectX::FXMVECTOR position);
	

private:
	std::vector<Layer> layers;
};