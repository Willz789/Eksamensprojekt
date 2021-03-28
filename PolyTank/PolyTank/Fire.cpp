#include "Fire.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "BlendState.h"
#include "DepthState.h"

#include <random>
#include <set>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

void FireParticle::getBindables(Graphics& gfx, ParticleSystem<FireParticle>& ps) {
	constexpr Vertex vertices[] = {
		{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +1.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f, +1.0f }, { 0.0f, 1.0f } }
	};

	constexpr D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INST_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_RADIUS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	ComPtr<ID3DBlob> pVSBlob;

	ps.addBindable(gfx.getBindMgr()->get<VertexBuffer>("fireParticle", vertices, std::size(vertices), sizeof(Vertex)));
	ps.addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/FireVS.cso", &pVSBlob));
	ps.addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/FirePS.cso"));
	ps.addBindable(gfx.getBindMgr()->get<InputLayout>(inputElements, std::size(inputElements), pVSBlob.Get()));
	ps.addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	ps.addBindable(gfx.getBindMgr()->get<BlendState>(BlendState::Mode::BLEND));
	ps.addBindable(gfx.getBindMgr()->get<DepthState>(true));
}

Fire::Fire(Graphics& gfx, SceneNode* pEmitter, DirectX::FXMVECTOR emissionPos) :
	ParticleSystem<FireParticle>(gfx, 128),
	pEmitter(pEmitter) {

	XMStoreFloat3(&this->emissionPos, emissionPos);
}

FireParticle Fire::generateParticle() {

	static std::random_device rd;
	static std::default_random_engine rng(rd());
	
	FireParticle p;
	
	XMVECTOR emitPos = XMVectorSetW(XMLoadFloat3(&emissionPos), 1.0f);

	XMStoreFloat3(&p.pos, XMVector4Transform(emitPos, pEmitter->localToWorld()));

	constexpr float pi = 3.14159265359f;
	float theta = std::uniform_real_distribution(0.0f, 2.0f * pi)(rng);
	float r = std::sqrtf(std::uniform_real_distribution(0.0f, 0.1f * 0.1f)(rng));

	XMStoreFloat3(&p.vel, XMVectorSet(
		r * std::sinf(theta),
		std::uniform_real_distribution(0.2f, 0.4f)(rng),
		r * std::cosf(theta),
		0.0f
	));
	p.lifetime = std::uniform_real_distribution(4.0f, 6.0f)(rng);

	return p;
}

void Fire::updateParticle(FireParticle& p, float dt) {
	p.lifetime -= dt;

	XMStoreFloat3(&p.pos, XMLoadFloat3(&p.pos) + dt * XMLoadFloat3(&p.vel));
	
	if (p.lifetime < 0) {
		p = generateParticle();
	}
}

FireParticle::Instance Fire::getInstance(const FireParticle& p) const {
	FireParticle::Instance drawData;
	drawData.pos = p.pos;
	//drawData.radius = -0.04f * p.lifetime * std::log((1.0f / 6.0f) * p.lifetime);
	drawData.radius = 0.1f * std::min(1.0f, p.lifetime);

	float lifetime3 = (p.lifetime * p.lifetime * p.lifetime) / (5.0f * 5.0f * 5.0f);

	drawData.col = {
		2.5f * lifetime3 + 0.02f,
		1.25f * lifetime3 + 0.02f,
		0.75f * lifetime3 + 0.02f,
		1.0f };

	return drawData;
}


