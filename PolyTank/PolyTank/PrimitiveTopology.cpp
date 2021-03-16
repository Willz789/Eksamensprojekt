#include "PrimitiveTopology.h"
#include "Graphics.h"

PrimitiveTopology::PrimitiveTopology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) :
	topology(topology) {}

void PrimitiveTopology::bind(Graphics& gfx) {
	gfx.getCtx()->IASetPrimitiveTopology(topology);
}
