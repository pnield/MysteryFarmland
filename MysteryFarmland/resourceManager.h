#pragma once

#include "lTexture.h"

class ResourceManager
{
public:
	LTexture txSpriteSheet;
	LTexture txGameText;

	ResourceManager();
	~ResourceManager();

	void free();

	int load(SDL_Renderer* gRenderer);
};
