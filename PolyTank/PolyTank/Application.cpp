#include "Application.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "PrimitiveTopology.h"
#include "GLTFLoader.h"

#include <chrono>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Application::Application() :
	wnd(1280, 720, "PolyTank"),
	gfx(wnd.getHwnd()),
	scene(gfx),
	menu(gfx)
	{
	
	wnd.setResizeCB([this](uint32_t w, uint32_t h) -> void {
		this->gfx.resize();
	});

	gfx.setCamera(XMMatrixLookAtRH(XMVectorSet(0, 1.0f, 5.0f, 1.0f), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
	GLTF::Loader("./Models/tank/tank.gltf").getScene(gfx, scene.getRoot());
	GLTF::Loader("./Models/ground/ground.gltf").getScene(gfx, scene.getRoot());

}

using std::chrono::steady_clock;

void Application::run() {
	
	wnd.setVisible(true);


	steady_clock::time_point tstart = steady_clock::now();
	steady_clock::time_point t0 = tstart;

	while (true) {
		static uint32_t frameCount = 0;
		frameCount++;

		steady_clock::time_point t1 = steady_clock::now();
		float t = std::chrono::duration<float>(t1 - tstart).count();
		float dt = std::chrono::duration<float>(t1 - t0).count();
		t0 = t1;

		if (Window::handleMessages()) {
			return;
		}

		scene.getRoot()->getChild(0)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), t));
		scene.getRoot()->getChild(0)->getChild(13)->setRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -2.0f * t));

		gfx.beginFrame();

		gfx.shadowPass(XMLoadFloat3(&scene.lighting.sun.direction));
		scene.draw(gfx);

		gfx.viewPass();
		scene.draw(gfx);

		menu.draw(gfx);

		gfx.endFrame();
	}
}
