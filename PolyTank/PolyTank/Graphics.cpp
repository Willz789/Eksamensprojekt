#include "Graphics.h"
#include "Util.h"

#include <stdexcept>

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct PassTransforms {
	XMFLOAT4X4 projection;
	XMFLOAT4X4 view;
	XMFLOAT4X4 shadowProjection;
	XMFLOAT4X4 shadowView;
};

struct ModelTransforms {
	XMFLOAT4X4 world;
	XMFLOAT4X4 normalsWorld;
};

Graphics::Graphics(Window& wnd) :
	bindMgr(*this) {

	D3D_FEATURE_LEVEL fLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferDesc.Width = 0;
	swapDesc.BufferDesc.Height = 0;
	swapDesc.BufferDesc.RefreshRate = {0,0};
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = wnd.getHwnd();
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = 0;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hRes = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		fLevels,
		2,
		D3D11_SDK_VERSION,
		&swapDesc,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	);

	if (hRes != S_OK) {
		throw std::runtime_error("Failed to create device and swapchain");
	}

	ComPtr<IDXGIDevice> pDeviceDXGI;
	pDevice.As(&pDeviceDXGI);

	D2D1_FACTORY_OPTIONS fo2d;
#ifdef _DEBUG
	fo2d.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
	fo2d.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
	tif(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(pFactory2D), &fo2d, &pFactory2D));

	tif(pFactory2D->CreateDevice(pDeviceDXGI.Get(), &pDevice2D));
	tif(pDevice2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pContext2D));
	
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(pFactoryWrite), &pFactoryWrite);

	wnd.getInteraction()->addListener([this](const ResizeEvent&) -> void {
		this->resize();
	});

	initShadowMap();

	perPassCBuf = VSConstantBuffer(*this, 0, sizeof(PassTransforms));
	perObjectCBuf = VSConstantBuffer(*this, 1, sizeof(ModelTransforms));
}

Graphics::~Graphics() {
	
	pFactoryWrite.Reset();
	
	pRT2D.Reset();
	pContext2D.Reset();
	pDevice2D.Reset();
	pFactory2D.Reset();

	pShadowVS.Reset();
	pShadowMapSampler.Reset();
	pShadowSRV.Reset();
	pShadowDSV.Reset();

	pDSV.Reset();
	pRTV.Reset();

	pContext.Reset();
	pDevice.Reset();

	pSwapChain.Reset();

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<IDXGIDebug> pDebug;
	tif(DXGIGetDebugInterface1(0, __uuidof(pDebug), &pDebug));
	tif(pDebug->ReportLiveObjects(DXGI_DEBUG_DX, DXGI_DEBUG_RLO_DETAIL));
#endif
}

void Graphics::resize()
{
	tif(pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

	DXGI_SWAP_CHAIN_DESC swapDesc;
	pSwapChain->GetDesc(&swapDesc);

	D3D11_TEXTURE2D_DESC textDesc;
	textDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;	
	textDesc.ArraySize = 1;
	textDesc.MipLevels = 1;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.SampleDesc.Count = 4;
	textDesc.SampleDesc.Quality = 0;
	textDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;
	textDesc.Width = swapDesc.BufferDesc.Width;
	textDesc.Height = swapDesc.BufferDesc.Height;

	ComPtr<ID3D11Texture2D> pText;
	tif(pDevice->CreateTexture2D(&textDesc, nullptr, &pText));
	
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;

	tif(pDevice->CreateRenderTargetView(pText.Get(), &rtvDesc, &pRTV));

	// Direct 2d render target init
	ComPtr<IDXGISurface> pSurface;
	pText.As(&pSurface);

	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	bp.dpiX = 0.0f;
	bp.dpiY = 0.0f;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = nullptr;

	tif(pContext2D->CreateBitmapFromDxgiSurface(pSurface.Get(), &bp, &pRT2D));

	pContext2D->SetTarget(pRT2D.Get());

	// depth stencil buffer init
	textDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tif(pDevice->CreateTexture2D(&textDesc, nullptr, &pText));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	tif(pDevice->CreateDepthStencilView(pText.Get(), &dsvDesc, &pDSV));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = float(swapDesc.BufferDesc.Width);
	viewport.Height = float(swapDesc.BufferDesc.Height);
	viewport.MinDepth = 0;
	viewport.MaxDepth = D3D11_MAX_DEPTH;

	XMStoreFloat4x4(&cameraProjection, XMMatrixPerspectiveFovRH(1.05f, viewport.Width / viewport.Height, 0.01f, 100.0f));
}

void Graphics::beginFrame()
{
	pContext2D->BeginDraw();
}

void Graphics::shadowPass(FXMVECTOR sunDir) {
	passType = PassType::SHADOW_PASS;

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	pContext->PSSetShaderResources(0, 1, &pNullSRV);

	pContext->ClearDepthStencilView(pShadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f,  0);
	pContext->OMSetRenderTargets(0, nullptr, pShadowDSV.Get());
	pContext->RSSetViewports(1, &shadowViewport);

	float shadowMapSize = 40.0f;
	float shadowMapDepth = 100.0f;
	XMMATRIX sunProjection = XMMatrixOrthographicRH(shadowMapSize, shadowMapSize, 0.0f, shadowMapDepth);

	XMMATRIX invCamView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&cameraView));
	XMVECTOR camPos = XMVector4Transform(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), invCamView);
	
	XMVECTOR sunPos = camPos - (shadowMapDepth / 2.0f) * sunDir;

	XMMATRIX sunView = XMMatrixLookAtRH(sunPos, camPos, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&shadowProjection, sunProjection);
	XMStoreFloat4x4(&shadowView, sunView);

	PassTransforms passTransforms;
	XMStoreFloat4x4(&passTransforms.projection, XMMatrixTranspose(sunProjection));
	XMStoreFloat4x4(&passTransforms.view, XMMatrixTranspose(sunView));
	
	perPassCBuf.update(*this, passTransforms);
}

void Graphics::viewPass() {
	passType = PassType::VIEW_PASS;

	float colArr[] = { 0.5f, 0.7f, 0.4f, 1.0f };

	pContext->ClearRenderTargetView(pRTV.Get(), colArr);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	pContext->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());
	pContext->RSSetViewports(1, &viewport);

	pContext->PSSetShaderResources(0, 1, pShadowSRV.GetAddressOf());
	pContext->PSSetSamplers(0, 1, pShadowMapSampler.GetAddressOf());

	PassTransforms passTransforms;
	XMStoreFloat4x4(&passTransforms.projection, XMMatrixTranspose(XMLoadFloat4x4(&cameraProjection)));
	XMStoreFloat4x4(&passTransforms.view, XMMatrixTranspose(XMLoadFloat4x4(&cameraView)));
	XMStoreFloat4x4(&passTransforms.shadowProjection, XMMatrixTranspose(XMLoadFloat4x4(&shadowProjection)));
	XMStoreFloat4x4(&passTransforms.shadowView, XMMatrixTranspose(XMLoadFloat4x4(&shadowView)));
	
	perPassCBuf.update(*this, passTransforms);
}

void Graphics::endFrame()
{
	pContext2D->EndDraw();

	ComPtr<ID3D11Resource> pRenderTarget;
	pRTV->GetResource(&pRenderTarget);
	
	ComPtr<ID3D11Resource> pBackBuffer;
	tif(pSwapChain->GetBuffer(0, __uuidof(pBackBuffer), &pBackBuffer));


	pContext->ResolveSubresource(pBackBuffer.Get(), 0, pRenderTarget.Get(), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	pSwapChain->Present(0, 0);
}

void Graphics::drawIndexed(size_t indexCount, DirectX::FXMMATRIX transform) {
	setTransform(transform);
	pContext->DrawIndexed(indexCount, 0, 0);
}

void Graphics::drawInstanced(size_t vertexCount, size_t instanceCount, DirectX::FXMMATRIX transform) {
	setTransform(transform);
	pContext->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void Graphics::setCamera(DirectX::FXMMATRIX cameraTransform) {
	XMStoreFloat4x4(&cameraView, cameraTransform);
}

DirectX::XMMATRIX Graphics::getCamera() {
	return XMLoadFloat4x4(&cameraView);
}

ID3D11Device* Graphics::getDvc() {
	return pDevice.Get();
}

ID3D11DeviceContext* Graphics::getCtx() {
	return pContext.Get();
}

BindableManager* Graphics::getBindMgr() {
	return &bindMgr;
}

ID2D1DeviceContext* Graphics::getCtx2D() {
	return pContext2D.Get();
}

IDWriteFactory* Graphics::getFactoryW()
{
	return pFactoryWrite.Get();
}

void Graphics::initShadowMap() {

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = 4096;
	texDesc.Height = 4096;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> pTex;
	tif(pDevice->CreateTexture2D(&texDesc, nullptr, &pTex));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2D.MipSlice = 0;

	tif(pDevice->CreateDepthStencilView(pTex.Get(), &dsvDesc, &pShadowDSV));
	
	shadowViewport.TopLeftX = 0.0f;
	shadowViewport.TopLeftY = 0.0f;
	shadowViewport.Width = float(texDesc.Width);
	shadowViewport.Height = float(texDesc.Height);
	shadowViewport.MinDepth = D3D11_MIN_DEPTH;
	shadowViewport.MaxDepth = D3D11_MAX_DEPTH;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	tif(pDevice->CreateShaderResourceView(pTex.Get(), &srvDesc, &pShadowSRV));

	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1.0f;
	sampDesc.MinLOD = std::numeric_limits<float>::min();
	sampDesc.MaxLOD = std::numeric_limits<float>::max();

	tif(pDevice->CreateSamplerState(&sampDesc, &pShadowMapSampler));

	ComPtr<ID3DBlob> pVSBlob;
	tif(D3DReadFileToBlob(L"./ShaderBin/ShadowVS.cso", &pVSBlob));
	tif(pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pShadowVS));
}

void Graphics::setTransform(DirectX::FXMMATRIX transform) {
	if (passType == PassType::SHADOW_PASS) {
		pContext->PSSetShader(nullptr, nullptr, 0);
	}

	ModelTransforms modelTransforms;

	XMStoreFloat4x4(&modelTransforms.world, XMMatrixTranspose(transform));
	XMStoreFloat4x4(
		&modelTransforms.normalsWorld,
		XMMatrixInverse(nullptr, transform)
	);

	modelTransforms.normalsWorld._41 = 0.0f;
	modelTransforms.normalsWorld._42 = 0.0f;
	modelTransforms.normalsWorld._43 = 0.0f;

	perObjectCBuf.update(*this, modelTransforms);

	perObjectCBuf.bind(*this);
	perPassCBuf.bind(*this);
}
