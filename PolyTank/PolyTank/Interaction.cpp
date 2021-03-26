#include "Interaction.h"

#include <queue>

bool Interaction::keyDown(char key) {
	return keysDown[key];
}

void Interaction::lMouseClick(uint32_t x, uint32_t y)
{
	MouseEvent e;
	e.button = MouseEvent::Button::LEFT;
	e.mousex = x;
	e.mousey = y;
	std::queue<MouseListener> executionQueue;
	for (auto it = mouseListeners.begin(); it != mouseListeners.end(); it++) {
		executionQueue.push(*it);
	}
	while (!executionQueue.empty()) {
		executionQueue.front()(e);
		executionQueue.pop();
	}
	
}

void Interaction::rMouseClick(uint32_t x, uint32_t y)
{
	MouseEvent e;
	e.button = MouseEvent::Button::RIGHT;
	e.mousex = x;
	e.mousey = y;
	std::queue<MouseListener> executionQueue;
	for (auto it = mouseListeners.begin(); it != mouseListeners.end(); it++) {
		executionQueue.push(*it);
	}
	while (!executionQueue.empty()) {
		executionQueue.front()(e);
		executionQueue.pop();
	}
}

void Interaction::keyClick(char key)
{
	KeyEvent e;
	e.key = key;
	std::queue<KeyListener> executionQueue;
	for (auto it = keyListeners.begin(); it != keyListeners.end(); it++) {
		executionQueue.push(*it);
	}
	while (!executionQueue.empty()) {
		executionQueue.front()(e);
		executionQueue.pop();
	}
}

MouseListener* Interaction::addListener(MouseListener ml)
{
	mouseListeners.push_back(ml);
	return &mouseListeners.back();
}

KeyListener* Interaction::addListener(KeyListener kl)
{
	keyListeners.push_back(kl);
	return &keyListeners.back();
}

void Interaction::removeListener(MouseListener* pml)
{
	for (auto it = mouseListeners.begin(); it != mouseListeners.end(); it++) {
		if (&*it == pml) {
			mouseListeners.erase(it);
			break;
		}
		
	}
}

void Interaction::removeListener(KeyListener* pkl)
{
	for (auto it = keyListeners.begin(); it != keyListeners.end(); it++) {
		if (&*it == pkl) {
			keyListeners.erase(it);
			break;
		}

	}
}
