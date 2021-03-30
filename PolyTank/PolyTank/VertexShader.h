#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <filesystem>
#include <unordered_map>

class VertexShader : public IBindable {
public:
	VertexShader() = default;
	VertexShader(Graphics& gfx, const std::filesystem::path& file, ID3DBlob** ppBlob = nullptr);

	void bind(Graphics& gfx) override;

	static std::string uid(const std::filesystem::path& file, ID3DBlob** ppBlob = nullptr);

private:
	static std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3DBlob>> blobs;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pShader;

};

