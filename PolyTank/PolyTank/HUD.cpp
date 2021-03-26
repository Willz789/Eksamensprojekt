#include "HUD.h"


using namespace D2D1;
using Microsoft::WRL::ComPtr;

HUD::HUD(Graphics& gfx, Interaction* pInteraction) :
	pInteraction(pInteraction),
	hpBar(RectF(0.0f, 0.0f, 0.0f, 0.0f)),
	ammoBar(RectF(0.0f, 0.0f, 0.0f, 0.0f)) {
	
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(0.0f, 0.0f, 0.0f), &pBlack));
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 0.0f, 0.0f), &pRed));
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 0.8f, 0.05f), &pOrange));

	resizeListener = pInteraction->addListener([this](const ResizeEvent& e) -> void {
		this->resize(e.width, e.height);
	});
}

HUD::~HUD() {
	pInteraction->removeListener(resizeListener);
}

void HUD::resize(uint32_t w, uint32_t h) {
	hpBar.left = float(w) * 0.25f;
	hpBar.right = float(w) * 0.75f;
	hpBar.top = h - 50;
	hpBar.bottom = hpBar.top + 15;

	ammoBar.left = hpBar.left;
	ammoBar.right = hpBar.right;
	ammoBar.top = hpBar.bottom;
	ammoBar.bottom = ammoBar.top + 15;

}

float map(float x, float a, float b, float c, float d) {
	x = (x - a) / (b - a); // [a, b] -> [0, 1]
	x = x * (d - c) + c; // [0, 1] -> [c, d]
	return x;
}

void HUD::draw(Graphics& gfx) {

	// TODO replace these
	constexpr float playerHP = 80.0f;
	constexpr float playerMaxHP = 100.0f;
	constexpr float playerAmmo = 40.0f;
	constexpr float playerMaxAmmo = 70.0f;

	D2D1_RECT_F hp;
	hp.left = hpBar.left;
	hp.right = map(playerHP, 0.0f, playerMaxHP, hpBar.left, hpBar.right);
	hp.top = hpBar.top;
	hp.bottom = hpBar.bottom;

	gfx.getCtx2D()->FillRectangle(hp, pRed.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(RoundedRect(hpBar, (hpBar.bottom - hpBar.top) / 2.0f, (hpBar.bottom - hpBar.top) / 2.0f), pBlack.Get(), 3.0f);


	D2D1_RECT_F ammo;
	ammo.left = ammoBar.left;
	ammo.right = map(playerAmmo, 0.0f, playerMaxAmmo, ammoBar.left, ammoBar.right);
	ammo.top = ammoBar.top;
	ammo.bottom = ammoBar.bottom;

	gfx.getCtx2D()->FillRectangle(ammo, pOrange.Get());
	gfx.getCtx2D()->DrawRectangle(ammoBar, pBlack.Get(), 3.0f);
}

