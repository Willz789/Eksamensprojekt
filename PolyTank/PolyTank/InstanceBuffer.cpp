#include "InstanceBuffer.h"

#include "Graphics.h"

InstanceBuffer::InstanceBuffer(Graphics& gfx, size_t instCount, size_t instSize) :
	instSize(instSize) {
	
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = instCount * instSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	tif(gfx.getDvc()->CreateBuffer(&desc, nullptr, &pBuf));
}

void InstanceBuffer::update(Graphics& gfx, void* pData, size_t instCount) {
	D3D11_MAPPED_SUBRESOURCE msr;
	tif(gfx.getCtx()->Map(pBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr));
	memcpy(msr.pData, pData, instCount * instSize);
	gfx.getCtx()->Unmap(pBuf.Get(), 0);
}

void InstanceBuffer::bind(Graphics& gfx) {
	UINT stride = instSize;
	UINT offset = 0;

	gfx.getCtx()->IASetVertexBuffers(1, 1, pBuf.GetAddressOf(), &stride, &offset);
}

size_t InstanceBuffer::getInstCount() {
	D3D11_BUFFER_DESC desc;
	pBuf->GetDesc(&desc);

	assert(desc.ByteWidth % instSize == 0);
	return desc.ByteWidth / instSize;
}
