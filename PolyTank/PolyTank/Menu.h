#pragma once

class PolyTank;

#include "Util.h"
#include "Graphics.h"
#include "Window.h"

class Menu
{
public:
	Menu(Graphics& gfx, PolyTank& polyTank);

	void draw(Graphics& gfx);



private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pWhiteBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlackBrush;

	Microsoft::WRL::ComPtr<IDWriteTextFormat> pWTFButtons;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pWTFTitle;

	MouseListener* buttonListener;
};

