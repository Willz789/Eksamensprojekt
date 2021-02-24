#pragma once

#include <string>
#include <stdint.h>
#include <unordered_map>

#include <Windows.h>

class Window {
public:
	Window(uint32_t width, uint32_t height, const std::string& title);
	~Window();

	void setVisible(bool visible);
	bool isVisible() const;
	
	static bool handleMessages();
	static LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static Window* getWndPtr(HWND hWnd);

private:
	void resize(uint32_t w, uint32_t h);
	

private:
	HWND hWnd;

	static std::unordered_map<HWND, Window*> hWndToWnd;
	static constexpr char wndClsName[] = "PolyTankWndCls";
};

