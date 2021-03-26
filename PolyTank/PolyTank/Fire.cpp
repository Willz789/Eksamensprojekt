#include "Fire.h"

#include <random>

using namespace DirectX;

Fire::Fire(Graphics& gfx, float spawnRadius, size_t particleCount) :
	ParticleSystem<FireParticle>(gfx, particleCount) {}

float uniform() {
	return std::rand() / float(RAND_MAX) * 2.0f - 1.0f;
}

XMVECTOR pointInUnitBall() {

	XMVECTOR p;
	
	do {
		p = XMVectorSet(uniform(), uniform(), uniform(), 0.0f);
	} while (XMVector3LengthSq(p).m128_f32[0] > 1.0f);

	return p;
}

FireParticle Fire::generateParticle() {
	FireParticle p;
	XMStoreFloat3(&p.pos, pointInUnitBall());
	p.vel = p.pos;
	

}
