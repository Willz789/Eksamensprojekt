#include "InputLayout.h"
#include "Graphics.h"
#include "Util.h"

#include <sstream>

InputLayout::InputLayout(
	Graphics& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements,
	ID3DBlob* pVSBlob
) : InputLayout(gfx, elements.data(), elements.size(), pVSBlob) {}

InputLayout::InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* pElements, size_t elementCount, ID3DBlob* pVSBlob) {
	tif(gfx.getDvc()->CreateInputLayout(
		pElements,
		elementCount,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pLayout
	));
}

void InputLayout::bind(Graphics& gfx) {
	gfx.getCtx()->IASetInputLayout(pLayout.Get());
}

std::string InputLayout::uid(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, ID3DBlob* pVSBlob) {
	return uid(elements.data(), elements.size(), pVSBlob);
}

std::string InputLayout::uid(const D3D11_INPUT_ELEMENT_DESC* pElements, size_t elementCount, ID3DBlob* pVSBlob) {
	std::stringstream ss("IL:");
	for (auto* pElement = pElements; pElement != pElements + elementCount; pElement++) {
		ss << pElement->SemanticName << pElement->SemanticIndex << ";";
	}
	return ss.str();
}

