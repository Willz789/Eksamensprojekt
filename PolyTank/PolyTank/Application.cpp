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
	gfx(wnd.getHwnd())
	{
	
	wnd.setResizeCB([this](uint32_t w, uint32_t h) -> void {
		this->gfx.resize();
	});

}

using std::chrono::steady_clock;

void Application::run() {
	
	wnd.setVisible(true);


	steady_clock::time_point tstart = steady_clock::now();
	steady_clock::time_point t0 = tstart;

	while (true) {
		static uint32_t frameCount = 0;
		frameCount++;

		if (Window::handleMessages()) {
			return;
		}

		steady_clock::time_point t1 = steady_clock::now();
		float t = std::chrono::duration<float>(t1 - tstart).count();
		float dt = std::chrono::duration<float>(t1 - t0).count();
		t0 = t1;
		
		update(t, dt);

		gfx.beginFrame();
		render();
		gfx.endFrame();
	}
}

Window* Application::getWnd()
{
	return &wnd;
}