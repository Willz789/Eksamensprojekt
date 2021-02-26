#pragma once

#include "Window.h"
#include "Graphics.h"

class Application {
public:
	Application();

	void run();

private:
	Window wnd;
	Graphics gfx;

};

