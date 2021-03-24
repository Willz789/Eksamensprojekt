#include "Menu.h"

using namespace D2D1;
using Microsoft::WRL::ComPtr;

Menu::Menu(Graphics& gfx)
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
}

void Menu::draw(Graphics& gfx)
{
	D2D1_RECT_F startGameRect = RectF(200, 450, 550, 550);
	D2D1_RECT_F endGameRect = RectF(1280 - 550, 450, 1280 - 200, 550);
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
	D2D1_RECT_F titleRect = RectF(1280/2-400, 50, 1280/2+400, 150);
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
