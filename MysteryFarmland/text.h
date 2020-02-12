#pragma once

#include "lTexture.h"

//normal space between characters is 5 pixels

class Text
{
public:
	SDL_Rect spriteSheetRect;//determined by the char
	int x;
	int y;

	Text(char* letter, int x, int y);
	~Text();

	void letterSprite(char* letter);//sets text's spritesheetrect to that matching the char

	void draw(SDL_Renderer* gRenderer, LTexture* txGameText);
};