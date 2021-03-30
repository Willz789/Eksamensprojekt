#include "Explosion.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "BlendState.h"
#include "DepthState.h"


#include <algorithm>
#include <random>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

void ExplosionParticle::getBindables(Graphics& gfx, ParticleSystem<ExplosionParticle>& ps) {

	constexpr float sqrt3 = 1.73205080757f;

	constexpr Vertex vertices[] = {
		{ { 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ { sqrt3 / 2.0f, -0.5f }, { sqrt3 / 2.0f, -0.5f  } },
		{ { -sqrt3 / 2.0f, -0.5f }, { -sqrt3 / 2.0f, -0.5f } },
	};

	constexpr D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INST_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_SCALE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_ANGLE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INST_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	ComPtr<ID3DBlob> pVSBlob;

	ps.addBindable(gfx.getBindMgr()->get<VertexBuffer>("explosionParticle", vertices, std::size(vertices), sizeof(Vertex)));
	ps.addBindable(gfx.getBindMgr()->get<VertexShader>("./ShaderBin/ExplosionVS.cso", &pVSBlob));
	ps.addBindable(gfx.getBindMgr()->get<PixelShader>("./ShaderBin/ExplosionPS.cso"));
	ps.addBindable(gfx.getBindMgr()->get<InputLayout>(inputElements, std::size(inputElements), pVSBlob.Get()));
	ps.addBindable(gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	ps.addBindable(gfx.getBindMgr()->get<BlendState>(BlendState::Mode::BLEND));
	ps.addBindable(gfx.getBindMgr()->get<DepthState>(true));
}

Explosion::Explosion(Graphics& gfx, SceneNode* pEmitter, DirectX::FXMVECTOR emitPos) :
	ParticleSystem<ExplosionParticle>(gfx, 256),
	pEmitter(pEmitter) {

	XMStoreFloat3(&this->emitPos, emitPos);

}

void Explosion::run() {
	resetParticles();
}

inline XMVECTOR randomPos() {
	static std::random_device rd;
	static std::default_random_engine rng(rd());
	static std::uniform_real_distribution dist(-1.0f, 1.0f);

	XMVECTOR pos;

	do {
		pos = XMVectorSet(dist(rng), dist(rng), dist(rng), 0.0f);
	} while (XMVectorGetX(XMVector3LengthSq(pos)) > 1.0f);

	return pos;
}

void Explosion::newParticle(ExplosionParticle& p) {
	static std::random_device rd;
	static std::default_random_engine rng(rd());
	
	constexpr float pi = 3.14159265359f;

	XMVECTOR pos = XMVectorSetW(XMLoadFloat3(&emitPos), 1.0f);
	pos += randomPos();
	pos = XMVector4Transform(pos, pEmitter->localToWorld());
	XMStoreFloat3(&p.pos, pos);

	XMVECTOR vel = 0.5f * randomPos();
	XMStoreFloat3(&p.vel, vel);

	p.angle = std::uniform_real_distribution(0.0f, 2.0f * pi)(rng);
	p.angleVel = std::uniform_real_distribution(0.0f, pi / 2.0f)(rng);
	p.lifetime = std::uniform_real_distribution(2.0f, 3.0f)(rng);
}

void Explosion::updateParticle(ExplosionParticle& p, float dt) {
	p.lifetime -= dt;
	
	p.angle += dt * p.angleVel;
	XMVECTOR vel = XMLoadFloat3(&p.vel);
	//vel += dt * XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
	XMStoreFloat3(&p.vel, vel);

	XMVECTOR pos = XMLoadFloat3(&p.pos);
	pos += dt * vel;
	XMStoreFloat3(&p.pos, pos);
	
}

inline XMFLOAT3 lerp(XMFLOAT3 c1, XMFLOAT3 c2, float x) {
	float xinv = 1.0f - x;

	return { x * c1.x + xinv * c2.x, x * c1.y + xinv * c2.y, x * c1.z + xinv * c2.z };
}

ExplosionParticle::Instance Explosion::getInstance(const ExplosionParticle& p) const {
	ExplosionParticle::Instance inst;

	inst.pos = p.pos;
	inst.angle = p.angle;
	inst.scale = std::max(0.0f, -0.5f * p.lifetime * p.lifetime + p.lifetime);


	XMFLOAT3 col = lerp({ 1.0f, 1.0f, 0.4f }, { 1.0f, 0.1f, 0.1f }, p.lifetime / 4.0f);
	inst.col = { col.x, col.y, col.z, 1.0f };

	return inst;
}

