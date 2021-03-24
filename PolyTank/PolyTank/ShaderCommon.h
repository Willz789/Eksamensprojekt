#pragma once

#include <d3dcompiler.h>
#include <wrl.h>
#include <filesystem>

namespace Shader {
	void read(const std::filesystem::path& file, ID3DBlob** ppBlob);
	void compile(const std::filesystem::path& file, const std::string& target, ID3DBlob** ppBlob);
}
