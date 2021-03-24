#pragma once

#include "Util.h"
#include "BindableManager.h"
#include "ConstantBuffer.h"

class Graphics
{
	enum class PassType {
		SHADOW_PASS, VIEW_PASS
	};

public:
	Graphics(HWND hwnd);
	void resize();

	void beginFrame();
	void shadowPass(DirectX::FXMVECTOR sunDir);
	void viewPass();
	void endFrame();

	void drawIndexed(size_t indexCount, DirectX::FXMMATRIX transform);

	void setCamera(DirectX::FXMMATRIX cameraTransform);
	DirectX::XMMATRIX getCamera();

	ID3D11Device* getDvc();
	ID3D11DeviceContext* getCtx();
	BindableManager* getBindMgr();

private:
	void initShadowMap();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pShadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShadowSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pShadowMapSampler;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pShadowVS;

	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT shadowViewport;

	BindableManager bindMgr;
	
	VSConstantBuffer perPassCBuf;
	VSConstantBuffer perObjectCBuf;

	DirectX::XMFLOAT4X4 cameraProjection;
	DirectX::XMFLOAT4X4 cameraView;
	DirectX::XMFLOAT4X4 shadowProjection;
	DirectX::XMFLOAT4X4 shadowView;

	PassType passType;
};

