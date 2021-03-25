#include "IndexBuffer.h"
#include "Graphics.h"
#include "Util.h"

#include <assert.h>

IndexBuffer::IndexBuffer(Graphics& gfx, const std::string& name, const std::vector<Index>& indices) {
	D3D11_BUFFER_DESC ibDesc;
	ibDesc.ByteWidth = sizeof(Index) * indices.size();
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	tif(gfx.getDvc()->CreateBuffer(&ibDesc, &data, &pBuf));

#ifdef _DEBUG
	std::string thisuid = uid(name, indices);
	tif(pBuf->SetPrivateData(WKPDID_D3DDebugObjectName, thisuid.size(), thisuid.c_str()));
#endif
}

void IndexBuffer::bind(Graphics& gfx) {
	gfx.getCtx()->IASetIndexBuffer(pBuf.Get(), DXGI_FORMAT_R32_UINT, 0);
}

size_t IndexBuffer::getIndexCount() const {
	
	D3D11_BUFFER_DESC desc;
	pBuf->GetDesc(&desc);

	assert(desc.ByteWidth % sizeof(Index) == 0);

	return desc.ByteWidth / sizeof(Index);

}

std::string IndexBuffer::uid(const std::string& name, const std::vector<Index>& indices) {
	if (name.empty()) {
		return std::string();
	} else {
		return "IB:" + name;
	}
}
