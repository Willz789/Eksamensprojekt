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
	// constants for different power-ups for player
	static constexpr uint8_t PowerUpNone = 0;
	static constexpr uint8_t PowerUpDamage = 1;

private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlack;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pRed;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pOrange;

	D2D1_ROUNDED_RECT hpBar;
	D2D1_ROUNDED_RECT ammoBar;
	D2D1_ELLIPSE minimapFrame;
	D2D1_ROUNDED_RECT powerUpFrame;

	Interaction* pInteraction;
	ResizeListener* resizeListener;

};

