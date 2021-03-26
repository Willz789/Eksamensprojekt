#include "PolyTank.h"
#include "GLTFLoader.h"

#include "InstanceBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFMaterial.h"

#include <random>

using namespace DirectX;

PolyTank::PolyTank() :
	scene(gfx),
	menu(gfx) {
	
	gfx.setCamera(XMMatrixLookAtRH(XMVectorSet(0, 1.0f, 5.0f, 1.0f), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());

	Drawable instanced;

	constexpr float pi = 3.14159265359f;

	std::vector<DefaultVertex> vertices;
	std::vector<Index> indices;

	for (uint32_t i = 0; i <= 12; i++) {
		float theta = i / 12.0f * pi;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (uint32_t j = 0; j < 24; j++) {
			float phi = j / 24.0f * 2.0f * pi;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			DefaultVertex v;
			v.position = { 0.05f * sinTheta * cosPhi, 0.05f * cosTheta, 0.05f * sinTheta * sinPhi };
			v.normal = v.position;
			v.texcoord = { j / 24.0f, i / 12.0f };

			vertices.push_back(v);
		}
		
		if (i != 0) {
			for (uint32_t j = (i-1) * 24; j < i * 24; j++) {
				indices.push_back(j);
				indices.push_back(j + 24);
				indices.push_back(j + 1);

				indices.push_back(j + 25);
				indices.push_back(j + 1);
				indices.push_back(j + 24);
			}
		}
	}

	struct InstanceData {
		XMFLOAT3 pos;
	};

	std::vector<InstanceData> instances;

	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_real_distribution uni(0.0f, 1.0f);

	for (size_t i = 0; i < 512; i++) {
		InstanceData inst;
		inst.pos = { 10 * uni(rng) - 5, 10 * uni(rng) - 5, 10 * uni(rng) - 5 };
		instances.push_back(inst);
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INST_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, sizeof(InstanceData) }
	};

	Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
	
	instanced.addBindable(gfx.getBindMgr()->get<VertexBuffer>("sphere", vertices));
	instanced.addBindable(gfx.getBindMgr()->get<IndexBuffer>("sphere", indices));
	instanced.addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/InstanceVS.cso", &pVSBlob));
	instanced.addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/PixelShader.cso"));
	instanced.addBindable(gfx.getBindMgr()->get<InputLayout>(elements, pVSBlob.Get()));
	instanced.addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	std::shared_ptr<GLTFMaterial> pMaterial = gfx.getBindMgr()->get<GLTFMaterial>();
	pMaterial->factors.baseColor = { 0.5f, 1.0f, 0.5f };
	instanced.addBindable(pMaterial);

	std::shared_ptr<InstanceBuffer> pInstBuf = std::make_shared<InstanceBuffer>(gfx, instances.size(), sizeof(InstanceData));
	pInstBuf->update(gfx, instances.data(), instances.size());
	instanced.addBindable(pInstBuf);

	SceneNode* pSphereNode = scene.getRoot()->addChild();
	pSphereNode->getMesh()->addDrawable(std::move(instanced));
}

void PolyTank::update(float t, float dt) {
	scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
	scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));
	scene.getRoot()->getChild(2)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
}

void PolyTank::render() {
	gfx.shadowPass(XMLoadFloat3(&scene.lighting.sun.direction));
	scene.draw(gfx);

	gfx.viewPass();
	scene.draw(gfx);

	menu.draw(gfx);
}
