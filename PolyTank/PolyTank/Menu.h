#pragma once

#include "Util.h"
#include "Graphics.h"
#include "Window.h"

class Menu
{
public:
	Menu(Graphics& gfx, Interaction* pInteraction);
	~Menu();

	void initListeners();
	void removeListeners();

	void resize(uint32_t w, uint32_t h);
	void draw(Graphics& gfx);

private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pWhiteBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlackBrush;

	Microsoft::WRL::ComPtr<IDWriteTextFormat> pWTFButtons;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pWTFTitle;

	D2D1_RECT_F titleRect;
	D2D1_RECT_F startGameRect;
	D2D1_RECT_F endGameRect;

	Interaction* pInteraction;
	MouseListener* pMListener;
	ResizeListener* pRListener;
};

