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
	gfx(wnd)
	{}

using std::chrono::steady_clock;

void Application::run() {
	
	wnd.setVisible(true);

	tstart = steady_clock::now();
	tframe = tstart;

	while (true) {
		static uint32_t frameCount = 0;
		frameCount++;

		if (Window::handleMessages()) {
			return;
		}

		steady_clock::time_point tnow = steady_clock::now();
		float t = std::chrono::duration<float>(tnow - tstart).count();
		float dt = std::chrono::duration<float>(tnow - tframe).count();
		tframe = tnow;
		
		update(t, dt);

		gfx.beginFrame();
		render();
		gfx.endFrame();
	}
}

void Application::resetTime()
{
	tstart = std::chrono::steady_clock::now();
	tframe = tstart;
}

Window* Application::getWnd()
{
	return &wnd;
}

Graphics& Application::getGfx()
{
	return gfx;
}
