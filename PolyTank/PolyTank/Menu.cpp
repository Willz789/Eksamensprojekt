#include "Menu.h"
#include "PolyTank.h"

#include <iostream>

using namespace D2D1;
using Microsoft::WRL::ComPtr;

inline bool isPointIn(D2D1_POINT_2F p, D2D1_RECT_F r) {
	return p.x > r.left && p.x < r.right&& p.y > r.top && p.y < r.bottom;
}

Menu::Menu(Graphics& gfx, Interaction* pInteraction) :
	pInteraction(pInteraction)
{
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(1.0f, 1.0f, 1.0f), &pWhiteBrush));
	tif(gfx.getCtx2D()->CreateSolidColorBrush(ColorF(0.0f, 0.0f, 0.0f), &pBlackBrush));

	gfx.getFactoryW()->CreateTextFormat(
		L"Agency FB",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50.0f,
		L"en-us",
		&pWTFButtons
	);
	pWTFButtons->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gfx.getFactoryW()->CreateTextFormat(
		L"Forte",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		80.0f,
		L"en-us",
		&pWTFTitle
	);
	pWTFTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	pRListener = pInteraction->addListener([this](const ResizeEvent& e) -> void {
		this->resize(e.width, e.height);
	});
}

Menu::~Menu() {
	if (pMListener) {
		pInteraction->removeListener(pMListener);
	}
	if (pRListener) {
		pInteraction->removeListener(pRListener);
	}
}

void Menu::initListeners()
{
	pMListener = pInteraction->addListener([this](const MouseEvent& e)->void {
		if (e.button == MouseEvent::Button::LEFT) {
			D2D1_POINT_2F mouseLocation = Point2F(e.mousex, e.mousey);
			if (isPointIn(mouseLocation, startGameRect)) {
				PolyTank::get().startGame();
			}
			else if (isPointIn(mouseLocation, endGameRect)) {
				PolyTank::get().getWnd()->exit();
			}
		}
	});
}

void Menu::removeListeners()
{
	if (pMListener) {
		pInteraction->removeListener(pMListener);
	}
}

void Menu::resize(uint32_t w, uint32_t h) {

	startGameRect.left = w * 4.0f / 24.0f;
	startGameRect.right = w * 10.0f / 24.0f;
	startGameRect.top = h * 15.0f / 24.0f;
	startGameRect.bottom = h * 18.0f / 24.0f;

	endGameRect.left = w * 14.0f / 24.0f;
	endGameRect.right = w * 20.0f / 24.0f;
	endGameRect.top = h * 15.0f / 24.0f;
	endGameRect.bottom = h * 18.0f / 24.0f;

	titleRect.left = 0.0f;
	titleRect.right = w;
	titleRect.top = h * 2.0f / 24.0f;
	titleRect.bottom = h * 5.0f / 24.0f;
}

void Menu::draw(Graphics& gfx)
{

	gfx.getCtx2D()->FillRoundedRectangle(RoundedRect(startGameRect, 10, 10), pWhiteBrush.Get());
	gfx.getCtx2D()->FillRoundedRectangle(RoundedRect(endGameRect, 10, 10), pWhiteBrush.Get());	
	gfx.getCtx2D()->DrawRoundedRectangle(RoundedRect(startGameRect, 10, 10), pBlackBrush.Get(), 3.0f);
	gfx.getCtx2D()->DrawRoundedRectangle(RoundedRect(endGameRect, 10, 10), pBlackBrush.Get(), 3.0f);

	// Text
	std::wstring startGameString(L"Start Game");
	std::wstring endGameString(L"Exit Game");

	ComPtr<IDWriteTextLayout> pWTL;
	gfx.getFactoryW()->CreateTextLayout(
		startGameString.c_str(), 
		startGameString.size(), pWTFButtons.Get(), 
		startGameRect.right-startGameRect.left,
		startGameRect.bottom - startGameRect.top, 
		&pWTL
	);

	pWTL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gfx.getCtx2D()->DrawTextLayout(Point2F(startGameRect.left, startGameRect.top), pWTL.Get(), pBlackBrush.Get());

	gfx.getFactoryW()->CreateTextLayout(
		endGameString.c_str(),
		endGameString.size(), pWTFButtons.Get(),
		endGameRect.right - endGameRect.left,
		endGameRect.bottom - endGameRect.top,
		&pWTL
	);

	pWTL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gfx.getCtx2D()->DrawTextLayout(Point2F(endGameRect.left, endGameRect.top), pWTL.Get(), pBlackBrush.Get());

	std::wstring titleString(L"Polytank");
	gfx.getFactoryW()->CreateTextLayout(
		titleString.c_str(),
		titleString.size(), pWTFTitle.Get(),
		titleRect.right - titleRect.left,
		titleRect.bottom - titleRect.top,
		&pWTL
	);
	pWTL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gfx.getCtx2D()->DrawTextLayout(Point2F(titleRect.left, titleRect.top), pWTL.Get(), pBlackBrush.Get());
}
