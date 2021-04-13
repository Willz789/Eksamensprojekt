#pragma once

// Disable warning about truncating size_t to smaller int type, as
// many win32 and DirectX functions take 32-bit types for sizes.
#pragma warning(disable : 4267)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef OPAQUE

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <DirectXMath.h>
#include <iostream>

#include <stdexcept>

inline void tif(HRESULT hres) {
	if (hres != S_OK) {
		throw std::runtime_error("Bad HRESULT");
	}
}

template<typename Enum>
std::underlying_type_t<Enum> toInteger(const Enum value) {
	return static_cast<std::underlying_type_t<Enum>>(value);
}

constexpr float pi = 3.14159265359f;

inline float normalizeAngle(float angle) {
	while (angle > 2.0f * pi) {
		angle -= 2.0f * pi;
	}

	while (angle < 0.0f) {
		angle += 2.0f * pi;
	}

	return angle;
}