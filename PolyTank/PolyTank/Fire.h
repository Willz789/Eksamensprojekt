#pragma once

#include "ParticleSystem.h"
#include "Scene.h"

class Fire;

struct FireParticle {

	struct Vertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT2 tex;
	};

	struct Instance {
		DirectX::XMFLOAT3 pos;
		float radius;
		DirectX::XMFLOAT4 col;
	};

	static void getBindables(Graphics& gfx, ParticleSystem<FireParticle>& ps);

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 vel;
	float lifetime;
};

class Fire : public ParticleSystem<FireParticle> {
public:
	Fire(Graphics& gfx, SceneNode* pEmitter, DirectX::FXMVECTOR emissionPos = { 0.0f, 0.0f, 0.0f, 1.0f });

protected:
	void newParticle(FireParticle& p) override;
	void updateParticle(FireParticle& p, float dt) override;
	FireParticle::Instance getInstance(const FireParticle& p) const override;
	
private:
	SceneNode* pEmitter;
	DirectX::XMFLOAT3 emissionPos;
};