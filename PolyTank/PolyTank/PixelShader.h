#pragma once
#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <filesystem>

class PixelShader : public IBindable {
public:
	PixelShader() = default;
	PixelShader(Graphics& gfx, const std::filesystem::path& file);

	void bind(Graphics& gfx) override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader;

};


