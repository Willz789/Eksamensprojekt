#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Scene.h"
#include "Menu.h"

class Application {
public:
	Application();

	void run();

private:
	Window wnd;
	Graphics gfx;
	Scene scene;
	Menu menu;

};

