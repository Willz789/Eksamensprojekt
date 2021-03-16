#pragma once

#include <Windows.h>
#include <stdexcept>

inline void tif(HRESULT hres) {
	if (hres != S_OK) {
		throw std::runtime_error("Bad HRESULT");
	}
}