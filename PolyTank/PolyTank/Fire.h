#pragma once

#include "ParticleSystem.h"
#include "Scene.h"

class Fire;

struct FireParticle {

	struct DrawData {
		DirectX::XMFLOAT3 pos;
		float radius;
		DirectX::XMFLOAT4 col;
	};

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 vel;
	float lifetime;
};

class Fire : public ParticleSystem<FireParticle> {
public:
	Fire(Graphics& gfx, float spawnRadius, size_t particleCount);

	FireParticle generateParticle() override;
	FireParticle::DrawData updateParticle(FireParticle& p, float dt) override;

	Drawable makeDrawable(Graphics& gfx);

private:
	float radius;
	bool stopped;

};