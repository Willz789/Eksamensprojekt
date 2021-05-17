#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <DirectXMath.h>
#include <vector>
#include <string>

struct DefaultVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoord;
};

class VertexBuffer : public IBindable {
public:
	VertexBuffer() = default;
	VertexBuffer(Graphics& gfx, const std::string& name, const std::vector<DefaultVertex>& vertices);
	VertexBuffer(Graphics& gfx, const std::string& name, const void* pData, size_t vertexCount, size_t vertexSize);

	void bind(Graphics& gfx) override;

	size_t getVertexCount();

	static std::string uid(const std::string& name, const std::vector<DefaultVertex>& vertices);
	static std::string uid(const std::string& name, const void* pData, size_t vertexCount, size_t vertexSize);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;
	size_t vertexSize;

};

