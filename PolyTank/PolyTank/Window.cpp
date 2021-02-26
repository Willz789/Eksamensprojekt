#include "Window.h"

#include <stdexcept>
#include <iostream>

std::unordered_map<HWND, Window*> Window::hWndToWnd;

Window::Window(uint32_t width, uint32_t height, const std::string& title) {
	
	WNDCLASSA wndCls;
	wndCls.style = 0;
	wndCls.lpfnWndProc = wndProc;
	wndCls.cbClsExtra = 0;
	wndCls.cbWndExtra = 0;
	wndCls.hInstance = GetModuleHandle(NULL);
	wndCls.hIcon = NULL;
	wndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndCls.hbrBackground = NULL;
	wndCls.lpszMenuName = NULL;
	wndCls.lpszClassName = wndClsName;

	RegisterClassA(&wndCls);

	const DWORD style = WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;

	RECT wndRect;
	wndRect.left = 0;
	wndRect.right = width;
	wndRect.top = 0;
	wndRect.bottom = height;

	AdjustWindowRect(&wndRect, style, false);

	hWnd = CreateWindowA(
		wndClsName,
		title.c_str(),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	if (hWnd == nullptr) {
		throw std::runtime_error("failed to create window.");
	}

	hWndToWnd[hWnd] = this;
}

Window::~Window() {
	hWndToWnd.erase(hWnd);
	DestroyWindow(hWnd);
	UnregisterClassA(wndClsName, GetModuleHandle(NULL));
}

HWND Window::getHwnd() const
{	
	return hWnd;
}

void Window::setVisible(bool visible) {
	ShowWindow(hWnd, visible ? SW_SHOW : SW_HIDE);
}

bool Window::isVisible() const {
	return IsWindowVisible(hWnd);
}

void Window::setResizeCB(std::function<void(uint32_t, uint32_t)> CB)
{
	resizeCB = CB;
}

bool Window::handleMessages() {
	MSG msg;

	while (PeekMessageA(&msg, NULL, 0, 0, true)) {

		if (msg.message == WM_QUIT) {
			return true;
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return false;
}

LRESULT Window::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		getWndPtr(hWnd)->resize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

Window* Window::getWndPtr(HWND hWnd) {
	return hWndToWnd[hWnd];
}

void Window::resize(uint32_t w, uint32_t h) {
	if (resizeCB) {
		resizeCB(w, h);
	}
}