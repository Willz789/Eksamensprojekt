#include "Fire.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"

#include <random>
#include <set>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Fire::Fire(Graphics& gfx, float spawnRadius, size_t particleCount) :
	ParticleSystem<FireParticle>(gfx, particleCount),
	radius(spawnRadius) {}

XMVECTOR pointInUnitBall() {

	static std::random_device rd;
	static std::default_random_engine rng(rd());
	static std::uniform_real_distribution dist(-1.0f, 1.0f);

	XMVECTOR p;
	
	do {
		p = XMVectorSet(dist(rng), dist(rng), dist(rng), 0.0f);
	} while (XMVector3LengthSq(p).m128_f32[0] > 1.0f);

	return p;
}

XMVECTOR pointOnUnitSphere() {
	return XMVector3Normalize(pointInUnitBall());
}

FireParticle Fire::generateParticle() {

	static std::random_device rd;
	static std::default_random_engine rng(rd());
	static std::uniform_real_distribution dist(4.0f, 6.0f);

	FireParticle p;
	XMStoreFloat3(&p.pos, radius * pointInUnitBall() + XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f));
	XMStoreFloat3(&p.vel, 0.1f * pointOnUnitSphere());
	p.lifetime = dist(rng);

	return p;
}

FireParticle::DrawData Fire::updateParticle(FireParticle& p, float dt) {
	p.lifetime -= dt;
	XMStoreFloat3(&p.pos, XMLoadFloat3(&p.pos) + dt * XMLoadFloat3(&p.vel));

	if (p.lifetime < 0) {
		p = generateParticle();
	}
	
	FireParticle::DrawData drawData;
	drawData.pos = p.pos;
	drawData.radius = -0.05f * p.lifetime * std::log((1.0f / 6.0f) * p.lifetime);

	drawData.col = { 
		(1.0f / 6.0f) * p.lifetime * p.lifetime,
		(0.5f / 6.0f) * p.lifetime * p.lifetime,
		(0.25f / 6.0f ) * p.lifetime * p.lifetime,
		std::min(1.0f, p.lifetime) };

	return drawData;
}

Drawable Fire::makeDrawable(Graphics& gfx) {
	Drawable drawable;

	struct ParticleVertex {
		XMFLOAT2 pos;
		XMFLOAT2 texcoord;
	};

	ParticleVertex vertices[] = {
		{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, +1.0f }, { 0.0f, 1.0f } },
		{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +1.0f, +1.0f }, { 1.0f, 1.0f } }
	};

	Index indices[] = {
		0, 1, 2, 3, 2, 1
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> ies = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INST_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_RADIUS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	ComPtr<ID3DBlob> pVSBlob;

	drawable.addBindable(gfx.getBindMgr()->get<VertexBuffer>("fireParticle", vertices, std::size(vertices), sizeof(ParticleVertex)));
	drawable.addBindable(gfx.getBindMgr()->get<IndexBuffer>("fireParticle", indices, std::size(indices)));
	drawable.addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/FireVS.cso", &pVSBlob));
	drawable.addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/FirePS.cso"));
	drawable.addBindable(gfx.getBindMgr()->get<InputLayout>(ies, pVSBlob.Get()));
	drawable.addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	drawable.addBindable(pInstBuf);

	return drawable;
}

