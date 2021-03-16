#include "InputLayout.h"
#include "Graphics.h"
#include "Util.h"


InputLayout::InputLayout(
	Graphics& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements,
	ID3DBlob* pBlob
) {
	tif(gfx.getDvc()->CreateInputLayout(
		elements.data(), elements.size(),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pLayout
	));
}

void InputLayout::bind(Graphics& gfx) {
	gfx.getCtx()->IASetInputLayout(pLayout.Get());
}

