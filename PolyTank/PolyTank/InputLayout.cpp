#include "InputLayout.h"
#include "Graphics.h"
#include "Util.h"

#include <sstream>

InputLayout::InputLayout(
	Graphics& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements,
	ID3DBlob* pBlob
) {
	tif(gfx.getDvc()->CreateInputLayout(
		elements.data(),
		elements.size(),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pLayout
	));
}

void InputLayout::bind(Graphics& gfx) {
	gfx.getCtx()->IASetInputLayout(pLayout.Get());
}

std::string InputLayout::uid(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, ID3DBlob* pBlob) {
	std::stringstream ss("IL:");
	for (auto& e : elements) {
		ss << e.SemanticName << e.SemanticIndex << ";";
	}
	return ss.str();
}

