#pragma once

#include "IBindable.h"
#include "Util.h"

class InstanceBuffer : public IBindable {
public:
	InstanceBuffer(Graphics& gfx, size_t instCount, size_t instSize);

	void update(Graphics& gfx, void* pData, size_t instCount);
	void bind(Graphics& gfx) override;

	size_t getInstCount();

	static std::string uid(size_t instCount, size_t instSize);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;
	size_t instSize;

};


