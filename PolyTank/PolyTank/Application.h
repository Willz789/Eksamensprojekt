#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Physics.h"
#include "Scene.h"

class Application {
public:
	Application();

	void run();

	virtual void update(float t, float dt) = 0;
	virtual void render() = 0;
	
	Window* getWnd();

protected:
	Window wnd;
	Graphics gfx;
	Physics pcs;
};


