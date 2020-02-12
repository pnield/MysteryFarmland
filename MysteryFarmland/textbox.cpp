#include "textbox.h"

Textbox::Textbox(int x, int y, int width, int height)
{
	resize(x, y, width, height);
}

Textbox::~Textbox()
{
}

void Textbox::resize(int& x, int& y, int& width, int& height)
{
	textboxRect.x = x;
	textboxRect.y = y;
	textboxRect.w = width;
	textboxRect.h = height;
}

void Textbox::draw(SDL_Renderer* gRenderer)
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &textboxRect);
}

void Textbox::drawBorder(SDL_Renderer* gRenderer)
{
	SDL_SetRenderDrawColor(gRenderer, 29, 43, 83, 255);
	SDL_RenderDrawRect(gRenderer, &textboxRect);
}