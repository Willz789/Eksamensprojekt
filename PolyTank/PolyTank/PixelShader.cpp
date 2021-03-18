#include "PixelShader.h"
#include "Graphics.h"
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

#ifdef _DEBUG
	std::string thisuid = uid(file);
	tif(pShader->SetPrivateData(WKPDID_D3DDebugObjectName, thisuid.size(), thisuid.c_str()));
#endif
}

void PixelShader::bind(Graphics& gfx) {
	gfx.getCtx()->PSSetShader(pShader.Get(), nullptr, 0);
}

std::string PixelShader::uid(const std::filesystem::path& file) {
	return "PS:" + file.string();
}
