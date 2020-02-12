#include "baseThing.h"

BaseThing::BaseThing()
{
	spriteSheetRect.w = 8;
	spriteSheetRect.h = 8;

	gameViewDrawX = 0;
	gameViewDrawY = 0;
	drawXRelative = 0;
	drawYRelative = 0;
	rumble = 0;
	visible = false;
}

BaseThing::BaseThing(int x, int y, int spriteSheetRectX, int spriteSheetRectY)
{
	this->spriteSheetRect.x = spriteSheetRectX;
	this->spriteSheetRect.y = spriteSheetRectY;
	spriteSheetRect.w = 8;
	spriteSheetRect.h = 8;
	visible = true;

	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();
	gameViewDrawX = 0;
	gameViewDrawY = 0;
	drawXRelative = 0;
	drawYRelative = 0;
	rumble = 0;
}

int BaseThing::getRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());//seeds the random number generator
	return std::uniform_int_distribution<>(min, max)(gen);
}

void BaseThing::setDrawXY()
{
	drawX = x * spriteSheetRect.w;
	drawY = y * spriteSheetRect.h;
}

void BaseThing::setLastXYOnMove(int targetX, int targetY)
{
	if (x + targetX != x)
		lastX = x;
	if (y + targetY != y)
		lastY = y;
}

void BaseThing::drawIfOnscreen(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	int baseDrawX = x * spriteSheetRect.w;
	int baseDrawY = y * spriteSheetRect.h;

	if (baseDrawX + spriteSheetRect.w + spriteSheetRect.w < gameView.x ||
		baseDrawX - spriteSheetRect.w > gameView.x + gameView.w ||
		baseDrawY + spriteSheetRect.h + spriteSheetRect.h < gameView.y ||
		baseDrawY - spriteSheetRect.h > gameView.y + gameView.h)
		visible = false;
	else
		visible = true;

	gameViewDrawX = baseDrawX - gameView.x;
	gameViewDrawY = baseDrawY - gameView.y;

	drawBaseThing(gRenderer, txSpriteSheet);
}

void BaseThing::drawMapPosition(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	int baseDrawX = x * spriteSheetRect.w;
	int baseDrawY = y * spriteSheetRect.h;

	if (baseDrawX + spriteSheetRect.w + spriteSheetRect.w < gameView.x ||
		baseDrawX - spriteSheetRect.w > gameView.x + gameView.w ||
		baseDrawY + spriteSheetRect.h + spriteSheetRect.h < gameView.y ||
		baseDrawY - spriteSheetRect.h > gameView.y + gameView.h)
		visible = false;

	int lastXDraw = lastX * spriteSheetRect.w;
	int lastYDraw = lastY * spriteSheetRect.h;

	if (drawX < baseDrawX)
	{
		if (drawX < lastXDraw)
			drawX = lastXDraw;

		drawX += (baseDrawX - lastXDraw) / 8;

		if (drawX > baseDrawX ||
			lastXDraw > baseDrawX)
			drawX = baseDrawX;
	}

	else if (drawX > baseDrawX)
	{
		if (drawX > lastXDraw)
			drawX = lastXDraw;

		drawX -= (lastXDraw - baseDrawX) / 8;

		if (drawX < baseDrawX ||
			lastXDraw < baseDrawX)
			drawX = baseDrawX;
	}

	if (drawY < baseDrawY)
	{
		if (drawY < lastYDraw)
			drawY = lastYDraw;

		drawY += (baseDrawY - lastYDraw) / 8;

		if (drawY > baseDrawY ||
			lastYDraw > baseDrawY)
			drawY = baseDrawY;
	}

	else if (drawY > baseDrawY)
	{
		if (drawY > lastYDraw)
			drawY = lastYDraw;

		drawY -= (lastYDraw - baseDrawY) / 8;

		if (drawY < baseDrawY ||
			lastYDraw < baseDrawY)
			drawY = baseDrawY;
	}

	gameViewDrawX = drawX - gameView.x;
	gameViewDrawY = drawY - gameView.y;

	drawBaseThing(gRenderer, txSpriteSheet);
}

void BaseThing::drawRealPosition(SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	gameViewDrawX = x;
	gameViewDrawY = y;

	drawBaseThing(gRenderer, txSpriteSheet);
}

void BaseThing::drawBaseThing(SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	if (rumble > 0)
	{
		int roundRumble = round(rumble);
		drawXRelative = getRandomInt(-roundRumble, roundRumble);
		drawYRelative = getRandomInt(-roundRumble, roundRumble);
	}

	if(visible == true)
		txSpriteSheet->renderClip(gameViewDrawX + drawXRelative, gameViewDrawY + drawYRelative, gRenderer, spriteSheetRect);

	rumble -= rumble / 20;

	if (drawXRelative > 0)
		drawXRelative -= 1;
	else if (drawXRelative < 0)
		drawXRelative += 1;

	if (drawYRelative > 0)
		drawYRelative -= 1;
	else if (drawYRelative < 0)
		drawYRelative += 1;

	if (rumble < 1)
		rumble = 0;
}