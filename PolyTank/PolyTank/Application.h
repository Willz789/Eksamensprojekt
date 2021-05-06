#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Physics.h"
#include "Scene.h"

#include <chrono>

class Application {
public:
	Application();

	void run();

	virtual void update(float t, float dt) = 0;
	virtual void render() = 0;
	
	void resetTime();

	Window* getWnd();

protected:
	Window wnd;
	Graphics gfx;
	Physics pcs;


	std::chrono::steady_clock::time_point tstart;
	std::chrono::steady_clock::time_point tframe;
};


