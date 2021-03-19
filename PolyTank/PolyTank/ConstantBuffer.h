#pragma once

#include "IBindable.h"
#include <d3d11.h>
#include <wrl.h>

#include <stdint.h>
#include <string>

class ConstantBuffer : public IBindable {
public:
	ConstantBuffer();
	ConstantBuffer(Graphics& gfx, uint32_t slot, size_t byteSize);

	void update(Graphics& gfx, const void* pValue, size_t valueSize);
	template<typename T>
	void update(Graphics& gfx, const T& value);

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;
	uint32_t slot;
};

template<typename T>
void ConstantBuffer::update(Graphics& gfx, const T& value) {
	update(gfx, &value, sizeof(T));
}

#define DECL_CBUF(SS)																	\
class SS##ConstantBuffer : public ConstantBuffer {										\
public:																					\
	using ConstantBuffer::ConstantBuffer;												\
																						\
	void bind(Graphics& gfx) override;													\
	static std::string uid(uint32_t slot, size_t byteSize);	\
};													

DECL_CBUF(VS)
DECL_CBUF(PS)

#undef DECL_CBUF