#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <stdint.h>
#include <vector>

using Index = uint32_t;

class IndexBuffer : public IBindable {
public:
	IndexBuffer() = default;
	IndexBuffer(Graphics& gfx, const std::vector<Index>& indices);

	void bind(Graphics& gfx) override;
	
	size_t getSize() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;

};

