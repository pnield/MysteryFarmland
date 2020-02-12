#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

class LTexture
{
public:
	SDL_Texture* myTexture;//the actual hardware texture
	int width;//image dimensions
	int height;

	LTexture();//initializes variables
	~LTexture();//deallocates memory

	bool loadFromFile(std::string path, SDL_Renderer* gRenderer);//loads image at specified path
	void free();//deallocates texture
	void renderClip(double x, double y, SDL_Renderer* gRenderer, SDL_Rect& spriteRect);//renders a part of a spritesheet
	void renderClipHorizontalFlip(double x, double y, SDL_Renderer* gRenderer, SDL_Rect& spriteRect);
};