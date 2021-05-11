#include "HUD.h"
#include "PolyTank.h"

using namespace D2D1;
using Microsoft::WRL::ComPtr;

HUD::HUD(Graphics& gfx, Interaction* pInteraction) :
	pInteraction(pInteraction),
	hpBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)),
	powerBar(RoundedRect(RectF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f)) 
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

	powerBar.rect.left = hpBar.rect.left;
	powerBar.rect.right = hpBar.rect.right;
	powerBar.rect.top = hpBar.rect.bottom;
	powerBar.rect.bottom = powerBar.rect.top + h * 0.02f;;
	powerBar.radiusX = (powerBar.rect.bottom - powerBar.rect.top) / 2.0f;
	powerBar.radiusY = powerBar.radiusX;
	
	minimapFrame.point.x = w * 21.0f / 24.0f;
	minimapFrame.point.y = h * 1.0f / 24.0f + w * 2.0f / 24.0f;
	minimapFrame.radiusX = w * 2.0f / 24.0f;
	minimapFrame.radiusY = minimapFrame.radiusX;

	powerUpFrame.rect.left = hpBar.rect.right + w * 0.01f;
	powerUpFrame.rect.right = powerUpFrame.rect.left + w * 0.03f;
	powerUpFrame.rect.top = hpBar.rect.top - h * 0.01f;
	powerUpFrame.rect.bottom = powerBar.rect.bottom + h * 0.01f;
	powerUpFrame.radiusX = powerBar.radiusX;
	powerUpFrame.radiusY = powerUpFrame.radiusX;
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
	float playerPower = PolyTank::get().getPlayer().getShotPower();
	float playerMaxPower = PolyTank::get().getPlayer().getMaxShotPower();

	D2D1_ROUNDED_RECT hp = hpBar;
	hp.rect.right = map(playerHP, 0.0f, playerMaxHP, hpBar.rect.left, hpBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(hp, pRed.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(hpBar, pBlack.Get(), 3.0f);

	D2D1_ROUNDED_RECT power = powerBar;
	power.rect.right = map(playerPower, 0.0f, playerMaxPower, powerBar.rect.left, powerBar.rect.right);

	gfx.getCtx2D()->FillRoundedRectangle(power, pOrange.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(powerBar, pBlack.Get(), 3.0f);

	gfx.getCtx2D()->DrawEllipse(minimapFrame, pBlack.Get(), 3.0f);
	gfx.getCtx2D()->DrawRoundedRectangle(powerUpFrame, pBlack.Get(), 2.0f);

	// Draw active power-up (Add more if more power-ups)
	switch (PolyTank::get().getPlayer().getActivePowerUp()) {
		case PowerUpDamage:
			gfx.getCtx2D()->DrawRoundedRectangle(powerUpFrame, pBlack.Get(), 2.0f);
			// Damage-symbol
		case PowerUpNone:
			std::cout << "No Power-up\n";
	}

}

