#include "Application.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFLoader.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Application::Application() :
	wnd(1280, 720, "PolyTank"), 
	gfx(wnd.getHwnd()) {
	
	wnd.setResizeCB([this](uint32_t w, uint32_t h) -> void {
		this->gfx.resize();
	});

	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
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

		scene.getRoot()->getChild(0)->rotate(XMQuaternionRotationAxis(XMVectorSet(0,1,0,0), 0.0001));
		scene.getRoot()->getChild(0)->getChild(13)->rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0.001));
		scene.draw(gfx);

		gfx.endFrame();
	}
}
