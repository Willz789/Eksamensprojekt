#include "Drawable.h"
#include "IndexBuffer.h"
#include "InstanceBuffer.h"
#include "Graphics.h"

using namespace DirectX;

Drawable::Drawable() : indexCount(0), instanceCount(1) {}

void Drawable::addBindable(std::shared_ptr<IBindable> bindable) {

	std::shared_ptr<IndexBuffer> pib = std::dynamic_pointer_cast<IndexBuffer>(bindable);
	if (pib) {
		indexCount = pib->getIndexCount();
	}

	std::shared_ptr<InstanceBuffer> pInstBuf = std::dynamic_pointer_cast<InstanceBuffer>(bindable);
	if (pInstBuf) {
		instanceCount = pInstBuf->getInstCount();
	}

	bindables.push_back(bindable);
}

void Drawable::draw(Graphics& gfx, FXMMATRIX transform) const {
	
	for (const auto& pb : bindables) {
		pb->bind(gfx);
	}

	gfx.draw(indexCount, instanceCount, transform);
}
