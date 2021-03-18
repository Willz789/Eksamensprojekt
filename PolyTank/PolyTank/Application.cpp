#include "Application.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Application::Application() :
	wnd(1280, 720, "PolyTank"), 
	gfx(wnd.getHwnd()) {
	
	wnd.setResizeCB([this](uint32_t w, uint32_t h) -> void {
		this->gfx.resize();
	});

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
	std::vector<Index> indices = {
		2, 1, 0, 1, 2, 3, // front
		5, 6, 4, 6, 5, 7, // back
		6, 3, 2, 3, 6, 7, // bottom
		1, 4, 0, 4, 1, 5, // top
		4, 2, 0, 2, 4, 6, // left
		3, 5, 1, 5, 3, 7  // right    
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	std::shared_ptr<VertexBuffer> pvb = gfx.getBindMgr()->get<VertexBuffer>("cube", vertices);
	std::shared_ptr<IndexBuffer> pib = gfx.getBindMgr()->get<IndexBuffer>("cube", indices);

	ComPtr<ID3DBlob> pBlob;
	std::shared_ptr<VertexShader> pvs = gfx.getBindMgr()->get<VertexShader>("./ShaderBin/VertexShader.cso", &pBlob);
	std::shared_ptr<PixelShader> pps = gfx.getBindMgr()->get<PixelShader>("./ShaderBin/PixelShader.cso");

	std::shared_ptr<InputLayout> pil = gfx.getBindMgr()->get<InputLayout>(inputElements, pBlob.Get());
	std::shared_ptr<PrimitiveTopology> ppt = gfx.getBindMgr()->get<PrimitiveTopology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cube.addBindable(pvb);
	cube.addBindable(pib);
	cube.addBindable(pvs);
	cube.addBindable(pps);
	cube.addBindable(pil);
	cube.addBindable(ppt);
}

void Application::run() {
	
	wnd.setVisible(true);

	while (true) {
		if (Window::handleMessages()) {
			return;
		}
		gfx.beginFrame();

		static uint32_t frameCount = 0;
		frameCount++;

		XMMATRIX transform = XMMatrixRotationY(frameCount * 0.001) * XMMatrixTranslation(0, 0, -5);
		cube.draw(gfx, transform);

		gfx.endFrame();
	}
}
