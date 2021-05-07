#include "Level.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFMaterial.h"
#include "BlendState.h"
#include "DepthState.h"
#include "Polyhedron.h"

#include <fstream>

using namespace DirectX;

inline bool isBlockDrivable(const std::vector<Layer>& layers, uint32_t i, uint32_t j, uint32_t k) {
	if (layers[i].doesBlockExist(j, k)) {
		return i + 1 >= layers.size() || !layers[i + 1].isBlockSolid(j,k);
	}
	return false;
}

Level::Level(Graphics& gfx, Physics& pcs, const std::filesystem::path& file, Scene& scene) 
{
	std::ifstream ifs(file, std::ios::binary);
	ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
	ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
	ifs.read(reinterpret_cast<char*>(&d), sizeof(d));
	std::vector<uint8_t> blocks(w*d);

	const XMVECTOR colors[] = {
		XMVectorSet(0.8f, 0.2f, 0.2f, 0.0f),
		XMVectorSet(0.2f, 0.8f, 0.2f, 0.0f),
		XMVectorSet(0.2f, 0.2f, 0.8f, 0.0f),
	};

	for (uint32_t i = 0; i < h; i++) {
		ifs.read(reinterpret_cast<char*>(blocks.data()), blocks.size());
		if (!ifs.good()) throw std::runtime_error("");

		SceneNode* pLayerNode = scene.getRoot()->addChild();
		layers.emplace_back(gfx, pcs, d, w, blocks, pLayerNode, colors[i % std::size(colors)]);
		layers.back().moveUp(i);
	}

	edges.resize(w*h*d*w*h*d);

	for (uint32_t i = 0; i < h; i++) {
		for (uint32_t j = 0; j < d; j++) {
			for (uint32_t k = 0; k < w; k++) {
				if (isBlockDrivable(layers, i, j, k)) {
					size_t col = i * d * w + j * w + k;

					for (int8_t jOff = -1; jOff < 2; jOff++) {
						for (int8_t kOff = -1; kOff < 2; kOff++) {
							uint32_t jNode = j + jOff;
							uint32_t kNode = k + kOff;
							if (jOff != 0 || kOff != 0) {
								if (jOff == 0 || kOff == 0) {
									if (isBlockDrivable(layers, i, jNode, kNode)) {
										size_t row = i * d * w + jNode * w + kNode;
										edges[row * w * h * d + col] = 1;
									}
								}
								else if(isBlockDrivable(layers, i, jNode, k) && isBlockDrivable(layers, i, j, kNode) && isBlockDrivable(layers, i, jNode, kNode)){
									size_t row = i * d * w + jNode * w + kNode;
									edges[row * w * h * d + col] = 1;
								}
							}
							
						}
					}
				}
			}
		}
	}
}

Layer* Level::getLayer(size_t idx) {
	return &layers[idx];
}

Layer* Level::getLayer(DirectX::FXMVECTOR position) {
	
	float y = XMVectorGetY(position);
	
	return &layers[std::clamp(static_cast<size_t>(y - 1.0f), 0ui64, layers.size() - 1)];
}

Node Level::findNearestNode(FXMVECTOR pos)
{
	float dist = std::numeric_limits<float>::infinity();
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	for (uint32_t i1 = 0; i1 < h; i1++) {
		for (uint32_t j1 = 0; j1 < d; j1++) {
			for (uint32_t k1 = 0; k1 < w; k1++) {
				if (isBlockDrivable(layers, i1, j1, k1)) {
					XMVECTOR nodeWorldSpace = XMVectorSet(k1 - w / 2.0f, i1, j1 - d / 2.0f, 0.0f);
					float distNew = XMVectorGetX(XMVector3LengthSq(pos - nodeWorldSpace));
					if (distNew < dist) {
						dist = distNew;
						i = i1;
						j = j1;
						k = k1;
					}
				}
			}
		}
	}

	Node node;
	node.i = i;
	node.j = j;
	node.k = k;

	return node;
}

uint32_t Level::getW()
{
	return w;
}

uint32_t Level::getD()
{
	return d;
}

uint32_t Level::getH()
{
	return h;
}

std::vector<uint8_t>* Level::getEdges()
{
	return &edges;
}

std::vector<Layer>* Level::getLayers()
{
	return &layers;
}

Layer::Layer(Graphics& gfx, Physics& pcs, uint32_t depth, uint32_t width, std::vector<uint8_t>& blocks, SceneNode* pNode, DirectX::FXMVECTOR color) :
	pNode(pNode)
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
				dv.position.x += j - w / 2.0f;
				dv.position.z += i - d / 2.0f;
				vertices.push_back(dv);
			}
			for (Index index : b.indices) {
				indices.push_back(baseIndex+index);
			}

			// add static body
			if (!b.vertices.empty()) {
				std::vector<XMFLOAT3> vertexPositions(b.vertices.size());
				
				for (const DefaultVertex& dv : b.vertices) {
					vertexPositions.push_back(dv.position);
				}
				
				blockBodies.push_back(pcs.emplaceBody<StaticBody>(
					std::make_unique<Polyhedron>(vertexPositions.data(), vertexPositions.size()),
					XMVectorSet(j - w / 2.0f, 0.0f, i - d / 2.0f, 0.0f),
					XMQuaternionIdentity()
				));
			}



		}
	}

	if (vertices.size() == 0) {
		this->pMesh = nullptr;
		return;
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
	DirectX::XMStoreFloat3(&pMaterial->factors.baseColor, color);
	pMesh->addBindable(pMaterial);

	this->pMesh = dynamic_cast<Mesh*>(pNode->addDrawable(std::move(pMesh)));

}

SceneNode* Layer::getNode() {
	return pNode;
}

void Layer::moveUp(uint32_t n) {
	
	XMVECTOR translation = XMVectorSet(0.0f, float(n), 0.0f, 0.0f);
	for (StaticBody* pBody : blockBodies) {
		pBody->setPosition(pBody->getPosition() + translation);
	}

	pNode->translate(translation);
}

bool Layer::doesBlockExist(uint32_t i, uint32_t j) const
{
	size_t pos = i * w + j;
	if (i >= d || j >= w) {
		return false;
	}
	return blocks[pos] != 0;
}

bool Layer::isBlockSolid(uint32_t i, uint32_t j) const
{
	if (i >= d || j >= w) {
		return false;
	}
	size_t pos = i * w + j;
	switch (blocks[pos]) {
	case emptyBlockId: 
		return false;
	case cubeBlockId:
		return true;
	case bridgeBlockId:
		return false;
	case ramp1BlockId:
		return true;
	case ramp2BlockId:
		return true;
	case liftBlockId:
		return true;
	}
}

std::vector<StaticBody*>* Layer::getBlockBodies()
{
	return &blockBodies;
}

uint8_t Layer::getBlockType(uint32_t i, uint32_t j)
{
	return blocks[i * w + j];
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
		{ { +0.5f, +0.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },

		// back
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, +0.0f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, +0.0f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } },

		// bottom
		{ { +0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right
		{ { +0.5f, +0.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.0f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -1.0f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		// left
		{ { -0.5f, +0.0f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
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
		{ { +0.5f, +0.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.2f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.2f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },

		// back
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.2f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -0.2f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, +0.0f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, +0.0f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } },

		// bottom
		{ { +0.5f, -0.2f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.2f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.2f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.2f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right
		{ { +0.5f, +0.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.2f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.0f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -0.2f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		// left
		{ { -0.5f, +0.0f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.2f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.2f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
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

inline Block makeRamp1Block() {
	Block b;
	b.vertices = {

		// back
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, +0.5f },  {0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 0.0f, 0.0f } },

		// bottom
		{ { +0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right
		{ { +0.5f, -1.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },

		// left
		{ { -0.5f, -1.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
	};

	b.indices = {
		// back
		0, 1, 2, 3, 2, 1,

		// top
		4, 5, 6, 7, 6, 5,
		
		// bottom
		8, 9, 10, 11, 10, 9,

		// right
		12, 13, 14,

		// left
		15, 16, 17,
	};

	return b;
}

inline Block makeRamp2Block() {
	Block b;
	b.vertices = {

		// front
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } },

		// back
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		// top 
		{ { +0.5f, +0.0f, -0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f },  {0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 1.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, +2.0f / (sqrtf(5.0f)), +1.0f / (sqrtf(5.0f)) }, { 0.0f, 0.0f } },

		// bottom
		{ { +0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		// right 
		{ { +0.5f, -0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { +0.5f, +0.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -1.0f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { +0.5f, -1.0f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },

		// left
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, +0.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -1.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -1.0f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		
	};

	b.indices = {
		// front
		0, 1, 2, 3, 2, 1,

		// back
		4, 5, 6, 7, 6, 5,

		// top
		8, 9, 10, 11, 10, 9,

		// bottom
		12, 13, 14, 15, 14, 13,

		// right
		16, 17, 18, 16, 19, 17, 20, 17, 19,

		// left
		21, 22, 23, 24, 21, 25, 23, 25, 21
	};

	return b;
}

int8_t intSin(uint8_t id) {
	return (0x1 & id) * ((0x2 & id) ? 0xff : 0x01);
}

int8_t intCos(uint8_t id) {
	return intSin(id + 1);
}

Block Layer::getBlock(uint8_t id) {

	static std::unordered_map<uint8_t, Block> blockMap = {
		{ 0, makeEmptyBlock() },
		{ 1, makeCubeBlock() },
		{ 2, makeBridgeBlock() },
		{ 3, makeRamp1Block() },
		{ 4, makeRamp2Block() },
	};

	uint8_t idRot = id >> 6;

	XMMATRIX rotation = XMMatrixSet(
 		 intCos(idRot), 0.0f, intSin(idRot), 0.0f,
				  0.0f, 1.0f,		   0.0f, 0.0f,
		-intSin(idRot), 0.0f, intCos(idRot), 0.0f,
				  0.0f, 0.0f,		   0.0f, 1.0f
	);

	Block b = blockMap[0xf & id];

	for (DefaultVertex& v : b.vertices) {
		XMStoreFloat3(&v.position, XMVector4Transform(XMLoadFloat3(&v.position), rotation));
		XMStoreFloat3(&v.normal, XMVector4Transform(XMLoadFloat3(&v.normal), rotation));
	}
	return b;
}
