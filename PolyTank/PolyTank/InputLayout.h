#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <vector>
#include <string>

class InputLayout : public IBindable {
public:
	InputLayout() = default;
	InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, ID3DBlob* pVSBlob);
	InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* pElements, size_t elementCount, ID3DBlob* pVSBlob);

	void bind(Graphics& gfx);

	static std::string uid(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, ID3DBlob* pVSBlob);
	static std::string uid(const D3D11_INPUT_ELEMENT_DESC* pElements, size_t elementCount, ID3DBlob* pVSBlob);

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;

};

