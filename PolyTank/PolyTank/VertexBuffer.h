#pragma once

#include "IBindable.h"
#include "Graphics.h"

#include <DirectXMath.h>
#include <vector>

struct Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 col;
};

class VertexBuffer : public IBindable {
public:
	VertexBuffer() = default;
	VertexBuffer(Graphics& gfx, const std::vector<Vertex>& vertices);

	void bind(Graphics& gfx) override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;

};

