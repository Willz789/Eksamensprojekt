#include "ConstantBuffer.h"
#include "Graphics.h"
#include "Util.h"

#include <sstream>

ConstantBuffer::ConstantBuffer(Graphics& gfx, uint32_t slot, size_t byteSize) : slot(slot) {
	
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	tif(gfx.getDvc()->CreateBuffer(&desc, nullptr, &pBuf));
}

void ConstantBuffer::update(Graphics& gfx, const void* pValue, size_t valueSize) {
	D3D11_MAPPED_SUBRESOURCE map;
	gfx.getCtx()->Map(pBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, pValue, valueSize);
	gfx.getCtx()->Unmap(pBuf.Get(), 0);
}

#define IMPL_CBUF(SS)															\
void SS##ConstantBuffer::bind(Graphics& gfx) {									\
	gfx.getCtx()->SS##SetConstantBuffers(slot, 1, pBuf.GetAddressOf());			\
}																				\
																				\
std::string SS##ConstantBuffer::uid(uint32_t slot, size_t byteSize) {			\
	return (std::stringstream("CB("#SS"):") << slot << ";" << byteSize).str();	\
}

IMPL_CBUF(VS)
IMPL_CBUF(PS)

#undef IMPL_CBUF