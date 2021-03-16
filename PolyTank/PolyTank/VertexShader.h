#pragma once

#include "IBindable.h"
#include "Graphics.h"

#include <filesystem>

class VertexShader : public IBindable {
public:
	VertexShader() = default;
	VertexShader(Graphics& gfx, const std::filesystem::path& file);

	void bind(Graphics& gfx) override;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pShader;

};

