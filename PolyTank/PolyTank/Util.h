#pragma once

#define NOMINMAX
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <DirectXMath.h>

#include <stdexcept>

inline void tif(HRESULT hres) {
	if (hres != S_OK) {
		throw std::runtime_error("Bad HRESULT");
	}
}