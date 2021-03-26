#pragma once

#include "Util.h"
#include "Graphics.h"

class HUD {
public:
	HUD(Graphics& gfx, Interaction* pInteraction);
	~HUD();

	void resize(uint32_t w, uint32_t h);
	void draw(Graphics& gfx);
	
private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlack;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pRed;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pOrange;

	D2D1_RECT_F hpBar;
	D2D1_RECT_F ammoBar;
	D2D1_RECT_F minimap;

	Interaction* pInteraction;
	ResizeListener* resizeListener;

};

