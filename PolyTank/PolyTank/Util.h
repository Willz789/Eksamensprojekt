#pragma once

// Disable warning about truncating size_t to smaller int type, as
// many win32 and DirectX functions take 32-bit types for sizes.
#pragma warning(disable : 4267)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <DirectXMath.h>

#include <stdexcept>

inline void tif(HRESULT hres) {
	if (hres != S_OK) {
		throw std::runtime_error("Bad HRESULT");
	}
}