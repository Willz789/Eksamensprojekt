#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <vector>

class InputLayout : public IBindable {
public:
	InputLayout() = default;
	InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, ID3DBlob* pBlob);

	void bind(Graphics& gfx);

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;

};

