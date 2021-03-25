#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Scene.h"
#include "Menu.h"

class Application {
public:
	Application();

	void run();

	virtual void update(float t, float dt) = 0;
	virtual void render() = 0;

protected:
	Window wnd;
	Graphics gfx;
};

