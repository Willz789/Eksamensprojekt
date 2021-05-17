#pragma once

#include "Graphics.h"
#include "IDrawable.h"

#include "InstanceBuffer.h"
#include "VertexBuffer.h"

#include <memory>
#include <vector>
#include <type_traits>

template<typename Particle>
class ParticleSystem : public IDrawable {
	using Instance = typename Particle::Instance;

public:
	ParticleSystem(Graphics& gfx, size_t particleCount);
	~ParticleSystem();

	void update(float t, float dt);
	void draw(Graphics& gfx, DirectX::FXMMATRIX transform) const;

	void addBindable(std::shared_ptr<IBindable> pBindable);

protected:
	void resetParticles();

	virtual void newParticle(Particle& p) = 0;
	virtual void updateParticle(Particle& p, float dt) = 0;
	virtual Instance getInstance(const Particle& p) const = 0;

private:
	size_t particleCount;
	size_t vertexCount;
	Particle* particles;
	Instance* instData;

	std::vector<std::shared_ptr<IBindable>> bindables;
	InstanceBuffer* pInstBuf;
};

template<typename Particle>
inline ParticleSystem<Particle>::ParticleSystem(Graphics& gfx, size_t particleCount) :
	particleCount(particleCount),
	particles(new Particle[particleCount]),
	instData(new Instance[particleCount]) {

	auto pInstBuf = std::make_shared<InstanceBuffer>(gfx, particleCount, sizeof(Instance));
	this->pInstBuf = pInstBuf.get();

	addBindable(pInstBuf);
	Particle::getBindables(gfx, *this);

}

template<typename Particle>
inline ParticleSystem<Particle>::~ParticleSystem() {
	delete[] particles;
	delete[] instData;
}

template<typename Particle>
inline void ParticleSystem<Particle>::update(float t, float dt) {
	for (size_t i = 0; i < particleCount; i++) {
		updateParticle(particles[i], dt);
	}
}

template<typename Particle>
inline void ParticleSystem<Particle>::draw(Graphics& gfx, DirectX::FXMMATRIX transform) const {
	for (size_t i = 0; i < particleCount; i++) {
		instData[i] = getInstance(particles[i]);
	}
	pInstBuf->update(gfx, instData, particleCount);

	for (auto& pb : bindables) {
		pb->bind(gfx);
	}

	gfx.drawInstanced(vertexCount, particleCount, transform);
}

template<typename Particle>
inline void ParticleSystem<Particle>::addBindable(std::shared_ptr<IBindable> pBindable) {
	auto pVB = std::dynamic_pointer_cast<VertexBuffer>(pBindable);
	if (pVB) {
		vertexCount = pVB->getVertexCount();
	}

	bindables.push_back(pBindable);
}

template<typename Particle>
inline void ParticleSystem<Particle>::resetParticles() {
	for (size_t i = 0; i < particleCount; i++) {
		newParticle(particles[i]);
	}
}
