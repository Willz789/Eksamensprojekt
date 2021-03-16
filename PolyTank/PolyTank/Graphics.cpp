#include "Graphics.h"
#include "Util.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <stdexcept>

#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


using Microsoft::WRL::ComPtr;
using namespace DirectX;

Graphics::Graphics(HWND hwnd)
{
	D3D_FEATURE_LEVEL fLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferDesc.Width = 0;
	swapDesc.BufferDesc.Height = 0;
	swapDesc.BufferDesc.RefreshRate = {0,0};
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = hwnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = 0;

	HRESULT hRes = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
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

	std::vector<Vertex> vertices = {
		{ { +1.0f, +1.0f, +1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -1.0f, +1.0f, +1.0f }, { 0.0f, 1.0f, 1.0f } },
		{ { +1.0f, -1.0f, +1.0f }, { 1.0f, 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, +1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { +1.0f, +1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -1.0f, +1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { +1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f } }
	};

	std::shared_ptr<VertexBuffer> pvb = std::make_shared<VertexBuffer>(*this, vertices);

	std::vector<Index> indices = {
		2, 1, 0, 1, 2, 3, // front
		5, 6, 4, 6, 5, 7, // back
		6, 3, 2, 3, 6, 7, // bottom
		1, 4, 0, 4, 1, 5, // top
		4, 2, 0, 2, 4, 6, // left
		3, 5, 1, 5, 3, 7  // right    
	};

	std::shared_ptr<IndexBuffer> pib = std::make_shared<IndexBuffer>(*this, indices);

	cube.addBindable(pvb);
	cube.addBindable(pib);

	return;
}

void Graphics::resize()
{
	pRTV.Reset();

	tif(pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

	ComPtr<ID3D11Resource> pBackBuffer;
	tif(pSwapChain->GetBuffer(0, __uuidof(pBackBuffer), &pBackBuffer));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	tif(pDevice->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &pRTV));
	
	DXGI_SWAP_CHAIN_DESC swapDesc;
	pSwapChain->GetDesc(&swapDesc);

	ComPtr<ID3D11Texture2D> pText;
	D3D11_TEXTURE2D_DESC textDesc;
	textDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textDesc.ArraySize = 1;
	textDesc.MipLevels = 1;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;
	textDesc.Width = swapDesc.BufferDesc.Width;
	textDesc.Height = swapDesc.BufferDesc.Height;
	tif(pDevice->CreateTexture2D(&textDesc, nullptr, &pText));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	tif(pDevice->CreateDepthStencilView(pText.Get(), &dsvDesc, &pDSV));

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = swapDesc.BufferDesc.Width;
	viewport.Height = swapDesc.BufferDesc.Height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = D3D11_MAX_DEPTH;

	pContext->RSSetViewports(1, &viewport);
}

void Graphics::beginFrame()
{
	float colArr[] = {
		0.5,
		0.7,
		0.4,
		1
	};

	pContext->ClearRenderTargetView(pRTV.Get(), colArr);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	pContext->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());

	
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	ComPtr<ID3DBlob> pBlob;
	tif(D3DReadFileToBlob(L"./ShaderBin/VertexShader.cso", &pBlob));
	ComPtr<ID3D11VertexShader> pVertexShader;
	tif(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);


	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ComPtr<ID3D11InputLayout> pLayout;
	tif(pDevice->CreateInputLayout(inputElements, std::size(inputElements), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pLayout));

	pContext->IASetInputLayout(pLayout.Get());

	tif(D3DReadFileToBlob(L"./ShaderBin/PixelShader.cso", &pBlob));
	ComPtr<ID3D11PixelShader> pPixelShader;
	tif(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(float) * 16*2;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;


	ComPtr<ID3D11Buffer> pcBuff;
	tif(pDevice->CreateBuffer(&cbDesc, nullptr, &pcBuff));

	static uint32_t frameCount = 0;
	frameCount++;

	struct {
		XMMATRIX transform = XMMatrixTranspose(XMMatrixRotationY(frameCount*0.001)*XMMatrixTranslation(0, 0, -5));
		XMMATRIX projection = XMMatrixTranspose(XMMatrixPerspectiveFovRH(1.05, 1.77777, 0.01, 1000));
	} dMatrix ;

	D3D11_MAPPED_SUBRESOURCE map;

	pContext->Map(pcBuff.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);

	memcpy(map.pData, &dMatrix, sizeof(dMatrix));
	pContext->Unmap(pcBuff.Get(), 0);
	pContext->VSSetConstantBuffers(0, 1, pcBuff.GetAddressOf());

	cube.draw(*this);
}

void Graphics::endFrame()
{
	pSwapChain->Present(0, 0);
}

ID3D11Device* Graphics::getDvc() {
	return pDevice.Get();
}

ID3D11DeviceContext* Graphics::getCtx() {
	return pContext.Get();
}
