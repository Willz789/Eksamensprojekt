#include "BlendState.h"

#include "Graphics.h"

#include <sstream>

BlendState::BlendState(Graphics& gfx, Mode mode) {
	
	D3D11_BLEND_DESC desc;
	desc.IndependentBlendEnable = false;

	switch (mode) {
	case Mode::OPAQUE:
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case Mode::ADD:
		desc.AlphaToCoverageEnable = true;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case Mode::BLEND:
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tif(gfx.getDvc()->CreateBlendState(&desc, &pState));
}

void BlendState::bind(Graphics& gfx) {
	static const float blendCol[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	gfx.getCtx()->OMSetBlendState(pState.Get(), blendCol, 0xffffffff);
}

std::string BlendState::uid(Mode mode) {
	return (std::stringstream() << "BS:" << toInteger(mode)).str();
}

