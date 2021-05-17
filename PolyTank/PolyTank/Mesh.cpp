#include "Mesh.h"

#include "IndexBuffer.h"

#include <iostream>

void Mesh::draw(Graphics& gfx, DirectX::XMMATRIX transform) const
{
	for (auto& pBindable : bindables) {
		pBindable->bind(gfx);
	}

	gfx.drawIndexed(indexCount, transform);
}

void Mesh::addBindable(std::shared_ptr<IBindable> pBindable) {

	std::shared_ptr<IndexBuffer> pib = std::dynamic_pointer_cast<IndexBuffer>(pBindable);
	if (pib) {
		indexCount = pib->getIndexCount();
	}

	bindables.push_back(pBindable);
}
