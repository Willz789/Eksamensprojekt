#pragma once

#include <bitset>
#include <list>
#include <functional>

struct MouseEvent {
	enum class Button{
		LEFT,
		RIGHT
	};

	uint32_t mousex, mousey;

	Button button;
};

struct KeyEvent {
	char key;
};

using MouseListener = std::function<void(const MouseEvent&)>;
using KeyListener = std::function<void(const KeyEvent&)>;

class Interaction
{
	friend class Window;
public:
	Interaction() = default;
	bool lMouseDown;
	bool rMouseDown;

	std::bitset<256> keysDown;
	bool keyDown(char key);

	MouseListener* addListener(MouseListener ml);
	KeyListener* addListener(KeyListener kl);

	void removeListener(MouseListener* pml);
	void removeListener(KeyListener* pkl);

	
private:
	std::list<MouseListener> mouseListeners;
	void lMouseClick(uint32_t x, uint32_t y);
	void rMouseClick(uint32_t x, uint32_t y);

	std::list<KeyListener> keyListeners;
	void keyClick(char key);


};

