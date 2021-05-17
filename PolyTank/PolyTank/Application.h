#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Physics.h"
#include "Scene.h"
#include "IGameObject.h"

#include <chrono>

class Application {
public:
	Application();

	void run();

	virtual void update(float t, float dt) = 0;
	virtual void render() = 0;
	
	void resetTime();
	
	IGameObject* addGameObject(std::unique_ptr<IGameObject>&& pGameObject);
	template <typename T, typename ...CTorArgs>
	T* emplaceGameObject(CTorArgs&&... args);
	void deleteGameObject(IGameObject* pGameObject);

	Window* getWnd();

	Graphics& getGfx();
	Physics& getPcs();

protected:
	Window wnd;
	Graphics gfx;
	Physics pcs;
	Scene scene;

	std::vector<IGameObject*> deleteList;
	std::vector<std::unique_ptr<IGameObject>> gameObjects;

	std::chrono::steady_clock::time_point tstart;
	std::chrono::steady_clock::time_point tframe;
};

template<typename T, typename ...CTorArgs>
inline T* Application::emplaceGameObject(CTorArgs&&... args)
{
	gameObjects.push_back(std::make_unique<T>(std::forward<CTorArgs>(args)...));
	return static_cast<T*>(gameObjects.back().get());
}
