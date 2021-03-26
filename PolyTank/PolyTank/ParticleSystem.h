#pragma once

#include "InstanceBuffer.h"

#include <memory>
#include <vector>
#include <type_traits>

template<typename Particle>
class ParticleSystem {
public:
	ParticleSystem(Graphics& gfx, size_t particleCount);
	
	void update(float t, float dt);
	std::shared_ptr<InstanceBuffer> getInstBuf() const;

protected:
	virtual Particle generateParticle() = 0;
	virtual Particle::DrawData updateParticle(Particle& p, float dt) = 0;

private:
	std::vector<Particle> particles;
	std::vector<Particle::DrawData> instData;
	std::shared_ptr<InstanceBuffer> pInstBuf;
};


template<typename Particle>
inline ParticleSystem<Particle>::ParticleSystem(Graphics& gfx, size_t particleCount) :
	particles(particleCount),
	instData(particleCount),
	pInstBuf(std::make_shared<InstanceBuffer>(gfx, particleCount, sizeof(Particle::DrawData))) {}

template<typename Particle>
inline void ParticleSystem<Particle>::update(float t, float dt) {
	for (size_t i = 0; i < particles.size(); i++) {
		instData[i] = updateParticle(particles[i], dt);
	}
}

template<typename Particle>
inline std::shared_ptr<InstanceBuffer> ParticleSystem<Particle>::getInstBuf() const {
	return pInstBuf;
}
