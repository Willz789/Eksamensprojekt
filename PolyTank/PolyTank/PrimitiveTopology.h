#pragma once
#include "IBindable.h"

#include <d3d11.h>

class PrimitiveTopology : public IBindable {
public:
	PrimitiveTopology() = default;
	PrimitiveTopology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology);

	void bind(Graphics& gfx) override;

private:
	D3D11_PRIMITIVE_TOPOLOGY topology;

};

