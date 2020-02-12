#pragma once

#include <random>
#include <memory>

#include "lTexture.h"

enum seasonList { seasonSpring, seasonSummer, seasonFall, seasonWinter };

class BaseThing
{
public:
	SDL_Rect spriteSheetRect;

	int x;//actual position
	int y;
	int lastX;//where you were last turn, for sprite lerping
	int lastY;
	double drawX;//the real location of the drawn sprite
	double drawY;
	int gameViewDrawX;//the drawn location relative to gameView
	int gameViewDrawY;
	int drawXRelative;//where sfx (shaking on hit, moving when attacking) move the sprite temporarily relative + or - from drawX/Y
	int drawYRelative;
	double rumble;//hit shake
	bool visible;

	BaseThing();
	BaseThing(int x, int y, int spriteSheetRectX, int spriteSheetRectY);

	int getRandomInt(int min, int max);
	void setDrawXY();
	void setLastXYOnMove(int targetX, int targetY);//setting lastX/Y so you appear to only move one tile since your last position

	void drawIfOnscreen(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);
	void drawMapPosition(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);
	void drawRealPosition(SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
	void drawBaseThing(SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
};