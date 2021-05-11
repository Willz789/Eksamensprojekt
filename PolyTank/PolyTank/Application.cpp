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
	gfx(wnd),
	scene(gfx)
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
		
		for (size_t i = 0; i < gameObjects.size(); i++) {
			gameObjects[i]->update(dt);
		}
		pcs.update(t, dt);
		update(t, dt); 

		size_t deleteListSize;
		do {
			deleteListSize = deleteList.size();
			gameObjects.erase(
				std::remove_if(gameObjects.begin(), gameObjects.end(), [this](std::unique_ptr<IGameObject>& g)->bool {
					return std::find(deleteList.begin(), deleteList.end(), g.get()) != deleteList.end();
				}), gameObjects.end()
			);
		} while (deleteList.size() != deleteListSize);

		deleteList.clear();

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

IGameObject* Application::addGameObject(std::unique_ptr<IGameObject>&& pGameObject)
{
	gameObjects.push_back(std::move(pGameObject));
	return gameObjects.back().get();
}

void Application::deleteGameObject(IGameObject* pGameObject)
{
	deleteList.push_back(pGameObject);
}

Window* Application::getWnd()
{
	return &wnd;
}

Graphics& Application::getGfx()
{
	return gfx;
}

Physics& Application::getPcs()
{
	return pcs;
}
