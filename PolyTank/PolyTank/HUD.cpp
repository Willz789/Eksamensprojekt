#include "HUD.h"
#include "PolyTank.h"

#include <sstream>

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
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 1.0f, 1.0f), &pWhite));

	resizeListener = pInteraction->addListener([this](const ResizeEvent& e) -> void {
		this->resize(e.width, e.height);
	});

	gfx.getFactoryW()->CreateTextFormat(
		L"Agency FB",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50.0f,
		L"en-us",
		&pWTFScore
	);
	pWTFScore->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gfx.getFactoryW()->CreateTextFormat(
		L"Agency FB",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"en-us",
		&pWTFPowerUp
	);
	pWTFPowerUp->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

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

	powerUpFrame.rect.left = hpBar.rect.right + w * 0.01f;
	powerUpFrame.rect.right = powerUpFrame.rect.left + w * 0.03f;
	powerUpFrame.rect.top = hpBar.rect.top - h * 0.01f;
	powerUpFrame.rect.bottom = powerBar.rect.bottom + h * 0.01f;
	powerUpFrame.radiusX = powerBar.radiusX;
	powerUpFrame.radiusY = powerUpFrame.radiusX;

	powerUpRect.left = powerUpFrame.rect.left;
	powerUpRect.right = powerUpFrame.rect.right;
	powerUpRect.top = powerUpFrame.rect.top;
	powerUpRect.bottom = powerUpFrame.rect.bottom;

	scoreRect.left = w / 2 - w * 0.5f;
	scoreRect.right = w / 2 + w * 0.5f;
	scoreRect.top = h * 2.0f / 25.0f;
	scoreRect.bottom = h * 5.0f / 24.0f;
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

	gfx.getCtx2D()->FillRoundedRectangle(powerUpFrame, pWhite.Get());
	gfx.getCtx2D()->DrawRoundedRectangle(powerUpFrame, pBlack.Get(), 2.0f);

	ComPtr<IDWriteTextLayout> pWTL;
	std::wstring powerUpString = (std::wstringstream() << L"x" << PolyTank::get().getPlayer().getDamageMultiplier()).str();
	gfx.getFactoryW()->CreateTextLayout(
		powerUpString.c_str(),
		powerUpString.size(),
		pWTFPowerUp.Get(),
		powerUpRect.right - powerUpRect.left,
		powerUpRect.bottom - powerUpRect.top,
		&pWTL
	);
	pWTL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gfx.getCtx2D()->DrawTextLayout(Point2F(powerUpRect.left, powerUpRect.top), pWTL.Get(), pBlack.Get());

	std::wstring scoreString = (std::wstringstream() << L"Score: " << PolyTank::get().getPlayer().getPoints()).str();
	gfx.getFactoryW()->CreateTextLayout(
		scoreString.c_str(),
		scoreString.size(),
		pWTFScore.Get(),
		scoreRect.right - scoreRect.left,
		scoreRect.bottom - scoreRect.top,
		&pWTL
	);
	pWTL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gfx.getCtx2D()->DrawTextLayout(Point2F(scoreRect.left, scoreRect.top), pWTL.Get(), pBlack.Get());

}

