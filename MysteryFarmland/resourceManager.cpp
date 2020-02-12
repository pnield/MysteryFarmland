#include "resourceManager.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	free();
}

void ResourceManager::free()
{
	txSpriteSheet.free();
	txGameText.free();
}

int ResourceManager::load(SDL_Renderer* gRenderer)
{
	if (!txSpriteSheet.loadFromFile("media/spritesheet.png", gRenderer))
	{
		return false;
	}

	if (!txGameText.loadFromFile("media/text.png", gRenderer))
	{
		return false;
	}

	return true;
}