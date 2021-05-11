#include "HUD.h"


using namespace D2D1;
using Microsoft::WRL::ComPtr;

HUD::HUD(Graphics& gfx, Interaction* pInteraction) :
	pInteraction(pInteraction),
	hpBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)),
	ammoBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)) 
{
	
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
	hpBar.rect.top = h * 0.9f;
	hpBar.rect.bottom = hpBar.rect.top + h * 0.02f;
	hpBar.radiusX = (hpBar.rect.bottom - hpBar.rect.top) / 2.0f;
	hpBar.radiusY = hpBar.radiusX;

	ammoBar.rect.left = hpBar.rect.left;
	ammoBar.rect.right = hpBar.rect.right;
	ammoBar.rect.top = hpBar.rect.bottom;
	ammoBar.rect.bottom = ammoBar.rect.top + h * 0.02f;
	ammoBar.radiusX = (ammoBar.rect.bottom - ammoBar.rect.top) / 2.0f;
	ammoBar.radiusY = ammoBar.radiusX;
	
	minimapFrame.point.x = w * 21.0f / 24.0f;
	minimapFrame.point.y = h * 1.0f / 24.0f + w * 2.0f / 24.0f;
	minimapFrame.radiusX = w * 2.0f / 24.0f;
	minimapFrame.radiusY = minimapFrame.radiusX;

	powerUpFrame.rect.left = ammoBar.rect.right + w * 0.01f;
	powerUpFrame.rect.right = powerUpFrame.rect.left + w * 0.03f;
	powerUpFrame.rect.top = hpBar.rect.top - h * 0.01f;
	powerUpFrame.rect.bottom = ammoBar.rect.bottom + h * 0.01f;
	powerUpFrame.radiusX = (ammoBar.rect.bottom - ammoBar.rect.top) / 2.0f;
	powerUpFrame.radiusY = powerUpFrame.radiusX;
}

// maps [a, b] -> [c, d]
inline float map(float x, float a, float b, float c, float d) {
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

	D2D1_ROUNDED_RECT hp = hpBar;
	hp.rect.right = map(playerHP, 0.0f, playerMaxHP, hpBar.rect.left, hpBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(hp, pRed.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(hpBar, pBlack.Get(), 3.0f);

	D2D1_ROUNDED_RECT ammo = ammoBar;
	ammo.rect.right = map(playerAmmo, 0.0f, playerMaxAmmo, ammoBar.rect.left, ammoBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(ammo, pOrange.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(ammoBar, pBlack.Get(), 3.0f);

	gfx.getCtx2D()->DrawEllipse(minimapFrame, pBlack.Get(), 3.0f);

	gfx.getCtx2D()->DrawRoundedRectangle(powerUpFrame, pBlack.Get(), 2.0f);

	// Draw active power-up (Add more if more power-ups)
	//switch (pPlayer->getActivePowerUp()) {
	//	case PowerUpDamage:
	//		gfx.getCtx2D()->DrawRoundedRectangle(powerUpFrame, pBlack.Get(), 2.0f);
	//		// Damage-symbol
	//	case PowerUpNone:
	//		std::cout << "No Power-up\n";
	//}

}

