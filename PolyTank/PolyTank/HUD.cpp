#include "HUD.h"
#include "PolyTank.h"

using namespace D2D1;
using Microsoft::WRL::ComPtr;

HUD::HUD(Graphics& gfx, Interaction* pInteraction) :
	pInteraction(pInteraction),
	hpBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)),
	ammoBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)) {
	
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(0.0f, 0.0f, 0.0f), &pBlack));
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 0.0f, 0.0f), &pRed));
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 0.6f, 0.05f), &pOrange));

	resizeListener = pInteraction->addListener([this](const ResizeEvent& e) -> void {
		this->resize(e.width, e.height);
	});
}

HUD::~HUD() {
	pInteraction->removeListener(resizeListener);
}

void HUD::resize(uint32_t w, uint32_t h) {

	hpBar.rect.left = float(w) * 0.25f;
	hpBar.rect.right = float(w) * 0.75f;
	hpBar.rect.top = h - 50;
	hpBar.rect.bottom = hpBar.rect.top + 15;
	hpBar.radiusX = (hpBar.rect.bottom - hpBar.rect.top) / 2.0f;
	hpBar.radiusY = hpBar.radiusX;

	ammoBar.rect.left = hpBar.rect.left;
	ammoBar.rect.right = hpBar.rect.right;
	ammoBar.rect.top = hpBar.rect.bottom;
	ammoBar.rect.bottom = ammoBar.rect.top + 15;
	ammoBar.radiusX = (ammoBar.rect.bottom - ammoBar.rect.top) / 2.0f;
	ammoBar.radiusY = ammoBar.radiusX;
	
	minimapFrame.point.x = w * 21.0f / 24.0f;
	minimapFrame.point.y = h * 1.0f / 24.0f + w * 2.0f / 24.0f;
	minimapFrame.radiusX = w * 2.0f / 24.0f;
	minimapFrame.radiusY = minimapFrame.radiusX;
}

// maps [a, b] -> [c, d]
inline float map(float x, float a, float b, float c, float d) {
	x = (x - a) / (b - a); // [a, b] -> [0, 1]
	x = x * (d - c) + c; // [0, 1] -> [c, d]
	return x;
}

void HUD::draw(Graphics& gfx) {
	float playerHP = float(PolyTank::get().getPlayer().getTank()->getHealth());
	float playerMaxHP = float(PolyTank::get().getPlayer().getTank()->getMaxHealth());
	float playerAmmo = 40.0f;
	float playerMaxAmmo = 70.0f;

	D2D1_ROUNDED_RECT hp = hpBar;
	hp.rect.right = map(playerHP, 0.0f, playerMaxHP, hpBar.rect.left, hpBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(hp, pRed.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(hpBar, pBlack.Get(), 3.0f);

	D2D1_ROUNDED_RECT ammo = ammoBar;
	ammo.rect.right = map(playerAmmo, 0.0f, playerMaxAmmo, ammoBar.rect.left, ammoBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(ammo, pOrange.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(ammoBar, pBlack.Get(), 3.0f);


	gfx.getCtx2D()->DrawEllipse(minimapFrame, pBlack.Get(), 3.0f);
}

