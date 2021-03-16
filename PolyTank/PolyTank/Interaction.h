#pragma once

#include <bitset>

class Interaction
{
public:
	Interaction() = default;
	bool lMouseDown;
	bool rMouseDown;

	std::bitset<256> keysDown;
	bool keyDown(char key);

	void lMouseClick();
	void rMouseClick();
	
};

