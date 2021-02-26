#include "Application.h"

Application::Application() :
	wnd(1280, 720, "PolyTank"), 
	gfx(wnd.getHwnd()){
	wnd.setResizeCB([this](uint32_t w, uint32_t h) -> void {
		this->gfx.resize();
	});
}

void Application::run() {
	
	wnd.setVisible(true);

	while (true) {
		if (Window::handleMessages()) {
			return;
		}
		gfx.beginFrame();
		gfx.endFrame();



	}
}
