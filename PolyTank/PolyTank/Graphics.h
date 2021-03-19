#pragma once

#include "Util.h"
#include "BindableManager.h"
#include "ConstantBuffer.h"

class Graphics
{
public:
	Graphics(HWND hwnd);
	void resize();
	void beginFrame();
	void endFrame();
	void drawIndexed(size_t indexCount, DirectX::FXMMATRIX transform);

	ID3D11Device* getDvc();
	ID3D11DeviceContext* getCtx();
	BindableManager* getBindMgr();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	BindableManager bindMgr;
	
	VSConstantBuffer perFrameCBuf;
	VSConstantBuffer perObjectCBuf;

};

