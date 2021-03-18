#include "VertexShader.h"
#include "Graphics.h"
#include "Util.h"
#include "ShaderCommon.h"

using Microsoft::WRL::ComPtr;

VertexShader::VertexShader(Graphics& gfx, const std::filesystem::path& file, ID3DBlob** ppBlob) {
	ComPtr<ID3DBlob> pBlob;
	
	if (file.extension() == ".hlsl") {
		Shader::compile(file, "vs_5_0", &pBlob);
	} else {
		Shader::read(file, &pBlob);
	}
	
	tif(gfx.getDvc()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pShader));

	if (ppBlob) {
		*ppBlob = pBlob.Detach();
	}

#ifdef _DEBUG
	std::string thisuid = uid(file, ppBlob);
	tif(pShader->SetPrivateData(WKPDID_D3DDebugObjectName, thisuid.size(), thisuid.c_str()));
#endif
}

void VertexShader::bind(Graphics& gfx) {
	gfx.getCtx()->VSSetShader(pShader.Get(), nullptr, 0);
}

std::string VertexShader::uid(const std::filesystem::path& file, ID3DBlob** ppBlob) {
	return "VS:" + file.string();
}
