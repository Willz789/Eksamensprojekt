#include "Drawable.h"
#include "IndexBuffer.h"
#include "Graphics.h"

void Drawable::addBindable(std::shared_ptr<IBindable> bindable) {

	std::shared_ptr<IndexBuffer> pib = std::dynamic_pointer_cast<IndexBuffer>(bindable);
	if (pib) {
		indexBufferSize = pib->getSize();
	}

	bindables.push_back(bindable);
}

void Drawable::draw(Graphics& gfx) const {
	
	for (const auto& pb : bindables) {
		pb->bind(gfx);
	}

	gfx.getCtx()->DrawIndexed(indexBufferSize, 0, 0);

}
