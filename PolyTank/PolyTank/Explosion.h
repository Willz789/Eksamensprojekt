#pragma once

#include "ParticleSystem.h"
#include "Scene.h"

struct ExplosionParticle {
	struct Vertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT2 tex;
	};

	struct Instance {
		DirectX::XMFLOAT3 pos;
		float scale;
		float angle;
		DirectX::XMFLOAT4 col;
	};

	static void getBindables(Graphics& gfx, ParticleSystem<ExplosionParticle>& ps);

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 vel;
	float angle;
	float angleVel;
	float lifetime;
};

class Explosion : public ParticleSystem<ExplosionParticle> {
public:
	Explosion(Graphics& gfx, SceneNode* pEmitter, DirectX::FXMVECTOR emitPos = { 0.0f, 0.0f, 0.0f, 1.0f });

	void run();

protected:
	void newParticle(ExplosionParticle& p) override;
	void updateParticle(ExplosionParticle& p, float dt) override;
	ExplosionParticle::Instance getInstance(const ExplosionParticle& p) const override;

private:
	SceneNode* pEmitter;
	DirectX::XMFLOAT3 emitPos;

};

