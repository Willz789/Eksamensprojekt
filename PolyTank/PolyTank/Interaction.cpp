#include "Interaction.h"

#include <queue>

Interaction::Interaction(HWND hWnd) :
	hWnd(hWnd),
	cursorLocked(false),
	cursorVisible(true) {}

bool Interaction::keyDown(char key) {
	return keysDown[key];
}

void Interaction::setCursorLocked(bool cursorLocked) {
	this->cursorLocked = cursorLocked;
}

void Interaction::setCursorVisible(bool cursorVisible) {
	if (this->cursorVisible ^ cursorVisible) {
		ShowCursor(cursorVisible);
		this->cursorVisible = cursorVisible;
	}
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

void Interaction::resize(uint32_t w, uint32_t h) {
	ResizeEvent e;
	e.width = w;
	e.height = h;

	std::queue<ResizeListener> executionQueue;
	for (auto it = resizeListeners.begin(); it != resizeListeners.end(); it++) {
		executionQueue.push(*it);
	}
	while (!executionQueue.empty()) {
		executionQueue.front()(e);
		executionQueue.pop();
	}
}

void Interaction::mouseMove(uint32_t x, uint32_t y)
{
	MouseEvent e;
	e.button = MouseEvent::Button::MOVE;

	if (cursorLocked) {
		if (hWnd != GetActiveWindow()) return;

		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(hWnd, &cursor);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		POINT center = {
			(clientRect.right) / 2,
			(clientRect.bottom) / 2
		};
		
		e.mousex = cursor.x - center.x;
		e.mousey = cursor.y - center.y;

		if (x == 0 && y == 0) {
			return;
		}

		ClientToScreen(hWnd, &center);
		SetCursorPos(center.x, center.y);
	} else {
		e.mousex = x;
		e.mousey = y;
	}


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

ResizeListener* Interaction::addListener(ResizeListener rl) {
	resizeListeners.push_back(rl);
	return &resizeListeners.back();
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

void Interaction::removeListener(ResizeListener* prl) {
	for (auto it = resizeListeners.begin(); it != resizeListeners.end(); it++) {
		if (&*it == prl) {
			resizeListeners.erase(it);
			break;
		}
	}
}
