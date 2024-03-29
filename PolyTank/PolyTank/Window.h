#pragma once

#include "Util.h"
#include "Interaction.h"

#include <string>
#include <stdint.h>
#include <unordered_map>
#include <functional>

class Window {
public:
	Window(uint32_t width, uint32_t height, const std::string& title);
	~Window();

	HWND getHwnd() const ;

	void setVisible(bool visible);
	bool isVisible() const;

	static bool handleMessages();
	static LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static Window* getWndPtr(HWND hWnd);

	Interaction* getInteraction();

	void exit();

private:
	HWND hWnd;
	Interaction interaction;

	static std::unordered_map<HWND, Window*> hWndToWnd;
	static constexpr char wndClsName[] = "PolyTankWndCls";
};

