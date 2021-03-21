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

	gfx.setCamera(XMMatrixLookAtRH(XMVectorSet(0, 1.0f, 5.0f, 1.0f), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());

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

		scene.getRoot()->getChild(0)->rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0.0001f));
		scene.getRoot()->getChild(0)->getChild(13)->rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0.001f));
		scene.draw(gfx);

		gfx.endFrame();
	}
}
