#pragma once

#include "IBindable.h"
#include "Util.h"
#include "ConstantBuffer.h"

#include <memory>
#include <string>

struct GLTFMaterialFactors {
	DirectX::XMFLOAT3 baseColor;
	float pad;
};

class GLTFMaterial : public IBindable {
public:
	GLTFMaterial(Graphics& gfx);

	void bind(Graphics& gfx);

	static std::string uid();

public:
	GLTFMaterialFactors factors;

private:
	std::shared_ptr<PSConstantBuffer> pCBuf;

};
