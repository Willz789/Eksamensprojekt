#include "ShaderCommon.h"
#include "Util.h"

using Microsoft::WRL::ComPtr;

void Shader::read(const std::filesystem::path& file, ID3DBlob** ppBlob) {
	tif(D3DReadFileToBlob(file.c_str(), ppBlob));
}

void Shader::compile(const std::filesystem::path& file, const std::string& target, ID3DBlob** ppBlob) {
	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrors;
	tif(D3DCompileFromFile(file.c_str(), nullptr, nullptr, "main", target.c_str(), flags, 0, ppBlob, &pErrors));

	if (pErrors) {
		throw std::runtime_error(static_cast<const char*>(pErrors->GetBufferPointer()));
	}
}
