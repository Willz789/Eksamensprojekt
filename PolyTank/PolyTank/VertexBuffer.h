#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <DirectXMath.h>
#include <vector>
#include <string>

struct Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 col;
};

class VertexBuffer : public IBindable {
public:
	VertexBuffer() = default;
	VertexBuffer(Graphics& gfx, const std::string& name, const std::vector<Vertex>& vertices);

	void bind(Graphics& gfx) override;

	static std::string uid(const std::string& name, const std::vector<Vertex>& vertices);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;

};

