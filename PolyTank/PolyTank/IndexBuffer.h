#pragma once

#include "IBindable.h"

#include <d3d11.h>
#include <wrl.h>

#include <stdint.h>
#include <vector>
#include <string>

using Index = uint32_t;

class IndexBuffer : public IBindable {
public:
	IndexBuffer() = default;
	IndexBuffer(Graphics& gfx, const std::string& name, const std::vector<Index>& indices);
	IndexBuffer(Graphics& gfx, const std::string& name, const Index* pData, size_t indexCount);

	void bind(Graphics& gfx) override;
	
	size_t getIndexCount() const;

	static std::string uid(const std::string& name, const std::vector<Index>& indices);
	static std::string uid(const std::string& name, const Index* pData, size_t indexCount);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;

};

