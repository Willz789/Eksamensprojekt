#include "VertexBuffer.h"
#include "Graphics.h"
#include "Util.h"

VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& name, const std::vector<DefaultVertex>& vertices) :
	VertexBuffer(gfx, name, vertices.data(), vertices.size(), sizeof(DefaultVertex)) {}

VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& name, const void* pData, size_t vertexCount, size_t vertexSize) : 
	vertexSize(vertexSize) {

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.ByteWidth = vertexCount * vertexSize;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	tif(gfx.getDvc()->CreateBuffer(&vbDesc, &data, &pBuf));

#ifdef _DEBUG
	std::string thisuid = uid(name, pData, vertexCount, vertexSize);
	tif(pBuf->SetPrivateData(WKPDID_D3DDebugObjectName, thisuid.size(), thisuid.c_str()));
#endif
}

void VertexBuffer::bind(Graphics& gfx) {
	UINT stride = vertexSize;
	UINT offset = 0;

	gfx.getCtx()->IASetVertexBuffers(0, 1, pBuf.GetAddressOf(), &stride, &offset);
}

std::string VertexBuffer::uid(const std::string& name, const std::vector<DefaultVertex>& vertices) {
	return "VB:" + name;
}

std::string VertexBuffer::uid(const std::string& name, const void* pData, size_t vertexCount, size_t vertexSize) {
	if (name.empty()) {
		return std::string();
	} else {
		return "VB:" + name;
	}
}
