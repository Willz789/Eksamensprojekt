#pragma once

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <wrl.h>


class Graphics
{
public:
	Graphics(HWND hwnd);
	void resize();
	void beginFrame();
	void endFrame();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;

};

