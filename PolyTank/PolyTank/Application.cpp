#include "Application.h"

Application::Application() :
	wnd(1280, 720, "PolyTank") {


}

void Application::run() {
	
	wnd.setVisible(true);

	while (true) {
		if (Window::handleMessages()) {
			return;
		}



	}
}
