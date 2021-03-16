#include "VertexShader.h"
#include "Util.h"
#include "ShaderCommon.h"

using Microsoft::WRL::ComPtr;

VertexShader::VertexShader(Graphics& gfx, const std::filesystem::path& file) {
	ComPtr<ID3DBlob> pBlob;
	
	if (file.extension() == ".hlsl") {
		Shader::compile(file, "vs_5_0", &pBlob);
	} else {
		Shader::read(file, &pBlob);
	}
	
	tif(gfx.getDvc()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader));
}

void VertexShader::bind(Graphics& gfx) {
	gfx.getCtx()->VSSetShader(pShader.Get(), nullptr, 0);
}
