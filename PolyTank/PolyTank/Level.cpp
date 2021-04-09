#include "Level.h"
#include <fstream>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFMaterial.h"
#include "BlendState.h"
#include "DepthState.h"

using namespace DirectX;

Level::Level(Graphics& gfx, const std::filesystem::path& file, Scene& scene)
{
	std::ifstream ifs(file, std::ios::binary);
	uint32_t w, h, d;
	ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
	ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
	ifs.read(reinterpret_cast<char*>(&d), sizeof(d));
	std::vector<uint8_t> blocks(w*d);
	for (uint32_t i = 0; i < h; i++) {
		ifs.read(reinterpret_cast<char*>(blocks.data()), blocks.size());
		SceneNode* pLayerNode = scene.getRoot()->addChild();
		pLayerNode->translate(XMVectorSet(0, i, 0, 0));
		layers.emplace_back(gfx, d, w, blocks, pLayerNode);
	}
}

Layer::Layer(Graphics& gfx, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pNode)
{
	this->blocks = blocks;
	d = depth;
	w = width;

	std::vector<DefaultVertex> vertices;
	std::vector<Index> indices;
	for (uint32_t i = 0; i < d; i++) {
		for (uint32_t j = 0; j < w; j++) {

			uint32_t idx = w * i + j;
			Block b = getBlock(blocks[idx]);
			Index baseIndex = vertices.size();
			for (DefaultVertex dv : b.vertices) {
				dv.position.x += j;
				dv.position.z += i;
				vertices.push_back(dv);
			}
			for (Index index : b.indices) {
				indices.push_back(baseIndex+index);
			}
		}
	}

	std::unique_ptr<Mesh> pMesh = std::make_unique<Mesh>();

	Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	pMesh->addBindable(std::make_shared<VertexBuffer>(gfx, "", vertices));
	pMesh->addBindable(std::make_shared<IndexBuffer>(gfx, "", indices));
	pMesh->addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/VertexShader.cso", &pVSBlob));
	pMesh->addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/PixelShader.cso"));
	pMesh->addBindable(gfx.getBindMgr()->get<InputLayout>(inputElements, pVSBlob.Get()));
	pMesh->addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	pMesh->addBindable(gfx.getBindMgr()->get<BlendState>(BlendState::Mode::OPAQUE));
	pMesh->addBindable(gfx.getBindMgr()->get<DepthState>(true));

	std::shared_ptr<GLTFMaterial> pMaterial = std::make_shared<GLTFMaterial>(gfx);
	pMaterial->factors.baseColor = { 0.8f, 0.8f, 0.8f };
	pMesh->addBindable(pMaterial);

	this->pMesh = dynamic_cast<Mesh*>(pNode->addDrawable(std::move(pMesh)));
}

inline Block makeEmptyBlock() 
{
	return Block();
}

inline Block makeCubeBlock()
{
	Block b;
	b.vertices = {
		// front
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },

		// back
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } },

		// bottom
		{ { +1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		// left
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f / 2.0f, -1.0f, +1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -1.0f, -1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
	};

	for (uint32_t i = 0; i < 6; i++) {
		b.indices.push_back(i * 4 + 0);
		b.indices.push_back(i * 4 + 1);
		b.indices.push_back(i * 4 + 2);
		b.indices.push_back(i * 4 + 3);
		b.indices.push_back(i * 4 + 2);
		b.indices.push_back(i * 4 + 1);
	}

	return b;
}

inline Block makeBridgeBlock()
{
	Block b;
	b.vertices = {
		// front
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },

		// back
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } },

		// bottom
		{ { +1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right
		{ { +1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		// left
		{ { -1.0f / 2.0f, +0.0f, +1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f / 2.0f, -0.2f, +1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f / 2.0f, +0.0f, -1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f / 2.0f, -0.2f, -1.0f / 2.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
	};

	for (uint32_t i = 0; i < 6; i++) {
		b.indices.push_back(i * 4 + 0);
		b.indices.push_back(i * 4 + 1);
		b.indices.push_back(i * 4 + 2);
		b.indices.push_back(i * 4 + 3);
		b.indices.push_back(i * 4 + 2);
		b.indices.push_back(i * 4 + 1);
	}

	return b;
}

Block Layer::getBlock(uint8_t id)
{
	static std::unordered_map<uint8_t, Block> blockMap = {
		{ 0, makeEmptyBlock() },
		{ 1, makeCubeBlock() },
		{ 2, makeBridgeBlock() }
	};
	return blockMap[id];
}