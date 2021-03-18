#pragma once
#include "IBindable.h"

#include <d3d11.h>
#include <string>

class PrimitiveTopology : public IBindable {
public:
	PrimitiveTopology() = default;
	PrimitiveTopology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology);

	void bind(Graphics& gfx) override;
	
	static std::string uid(D3D11_PRIMITIVE_TOPOLOGY topology);

private:
	D3D11_PRIMITIVE_TOPOLOGY topology;

};

