#include "VertexBuffer.h"
#include "Util.h"

VertexBuffer::VertexBuffer(Graphics& gfx, const std::vector<Vertex>& vertices) {
	
	D3D11_BUFFER_DESC vbDesc;
	vbDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	tif(gfx.getDvc()->CreateBuffer(&vbDesc, &data, &pBuf));
}

void VertexBuffer::bind(Graphics& gfx) {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	gfx.getCtx()->IASetVertexBuffers(0, 1, pBuf.GetAddressOf(), &stride, &offset);
}
