#include "GLTFMaterial.h"

#include "Graphics.h"

GLTFMaterial::GLTFMaterial(Graphics& gfx) :
	pCBuf(gfx.getBindMgr()->get<PSConstantBuffer>(1, sizeof(GLTFMaterialFactors))),
	factors({ { 1.0f, 1.0f, 1.0f } }) {}

void GLTFMaterial::bind(Graphics& gfx) {
	pCBuf->update(gfx, factors);
	pCBuf->bind(gfx);
}

std::string GLTFMaterial::uid() {
	return std::string();
}
