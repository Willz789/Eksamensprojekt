#pragma once

#include "Util.h"

#include <bitset>
#include <list>
#include <functional>

struct MouseEvent {
	enum class Button{
		LEFT,
		RIGHT,
		MOVE
	};

	int32_t mousex, mousey;

	Button button;
};

struct KeyEvent {
	char key;
};

struct ResizeEvent {
	uint32_t width, height;
};

using MouseListener = std::function<void(const MouseEvent&)>;
using KeyListener = std::function<void(const KeyEvent&)>;
using ResizeListener = std::function<void(const ResizeEvent&)>;

class Interaction
{
	friend class Window;
public:
	Interaction() = default;
	Interaction(HWND hWnd);

	bool keyDown(char key);

	void setCursorLocked(bool cursorLocked);
	void setCursorVisible(bool cursorVisible);

	MouseListener* addListener(MouseListener ml);
	KeyListener* addListener(KeyListener kl);
	ResizeListener* addListener(ResizeListener rl);

	void removeListener(MouseListener* pml);
	void removeListener(KeyListener* pkl);
	void removeListener(ResizeListener* prl);
	
private:

	void lMouseClick(uint32_t x, uint32_t y);
	void rMouseClick(uint32_t x, uint32_t y);
	void resize(uint32_t w, uint32_t h);

	void mouseMove(uint32_t x, uint32_t y);

	void keyClick(char key);

public:
	bool lMouseDown;
	bool rMouseDown;

	std::bitset<256> keysDown;

private:
	std::list<MouseListener> mouseListeners;
	std::list<KeyListener> keyListeners;
	std::list<ResizeListener> resizeListeners;

	bool cursorLocked;
	bool cursorVisible;

	HWND hWnd;
};

