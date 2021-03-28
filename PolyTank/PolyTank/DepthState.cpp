#include "DepthState.h"

#include "Graphics.h"

#include <sstream>

DepthState::DepthState(Graphics& gfx, bool writeDepth) {
	
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.StencilEnable = false;
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.DepthWriteMask = writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	
	tif(gfx.getDvc()->CreateDepthStencilState(&desc, &pState));
}

void DepthState::bind(Graphics& gfx) {
	gfx.getCtx()->OMSetDepthStencilState(pState.Get(), 0);
}

std::string DepthState::uid(bool writeDepth) {
	return (std::stringstream() << "DS:" << writeDepth).str();
}
