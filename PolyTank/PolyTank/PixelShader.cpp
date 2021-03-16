#include "PixelShader.h"
#include "Util.h"
#include "ShaderCommon.h"

using Microsoft::WRL::ComPtr;

PixelShader::PixelShader(Graphics& gfx, const std::filesystem::path& file) {
	ComPtr<ID3DBlob> pBlob;

	if (file.extension() == ".hlsl") {
		Shader::compile(file, "ps_5_0", &pBlob);
	} else {
		Shader::read(file, &pBlob);
	}

	tif(gfx.getDvc()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader));
}

void PixelShader::bind(Graphics& gfx) {
	gfx.getCtx()->PSSetShader(pShader.Get(), nullptr, 0);
}
