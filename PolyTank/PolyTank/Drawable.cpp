#include "Drawable.h"
#include "IndexBuffer.h"
#include "Graphics.h"

using namespace DirectX;

void Drawable::addBindable(std::shared_ptr<IBindable> bindable) {

	std::shared_ptr<IndexBuffer> pib = std::dynamic_pointer_cast<IndexBuffer>(bindable);
	if (pib) {
		indexBufferSize = pib->getSize();
	}

	bindables.push_back(bindable);
}

void Drawable::draw(Graphics& gfx, FXMMATRIX transform) const {
	
	for (const auto& pb : bindables) {
		pb->bind(gfx);
	}

	gfx.drawIndexed(indexBufferSize, transform);
}
