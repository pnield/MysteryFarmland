#include "lTexture.h"

LTexture::LTexture()
{
	myTexture = NULL;
	width = 0;
	height = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path, SDL_Renderer* gRenderer)
{
	free();//get rid of preexisting texture

	SDL_Texture* newTexture = NULL;//the final texture

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());//load image at specified path
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);//create texture from surface pixels
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			width = loadedSurface->w;//get image dimensions
			height = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);//get rid of old loaded surface
	}

	myTexture = newTexture;
	return myTexture != NULL;//return success
}

void LTexture::free()
{
	if (myTexture != NULL)//free texture if it exists
	{
		SDL_DestroyTexture(myTexture);
		myTexture = NULL;
	}
}

void LTexture::renderClip(double drawX, double drawY, SDL_Renderer* gRenderer, SDL_Rect& spriteRect)
{
	SDL_Rect renderQuad = { round(drawX), round(drawY), spriteRect.w, spriteRect.h };//set rendering space and render to screen
	SDL_RenderCopy(gRenderer, myTexture, &spriteRect, &renderQuad);//render to screen
}

void LTexture::renderClipHorizontalFlip(double drawX, double drawY, SDL_Renderer* gRenderer, SDL_Rect& spriteRect)
{
	SDL_Rect renderQuad = { round(drawX), round(drawY), spriteRect.w, spriteRect.h };
	SDL_RenderCopyEx(gRenderer, myTexture, &spriteRect, &renderQuad, NULL, NULL, SDL_FLIP_HORIZONTAL);//render to screen flipped horizontally
}