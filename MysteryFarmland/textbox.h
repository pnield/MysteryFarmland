#pragma once

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

class Textbox
{
public:
	SDL_Rect textboxRect;

	Textbox(int x, int y, int width, int height);
	~Textbox();

	void resize(int& x, int& y, int& width, int& height);

	void draw(SDL_Renderer* gRenderer);
	void drawBorder(SDL_Renderer* gRenderer);
};