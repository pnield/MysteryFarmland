#pragma once

#include <vector>
#include <memory>

#include "text.h"
#include "textbox.h"
#include "baseThing.h"

class Menu
{
public:
	std::unique_ptr<Textbox> textboxPtr;
	std::vector<std::unique_ptr<BaseThing>> iconArray;
	std::vector<std::unique_ptr<std::vector<std::unique_ptr<Text>>>> textArray;

	SDL_Rect menuRect;

	bool border;//does it have a border, for topMenu

	Menu();
	~Menu();

	void create(int x, int y, int width, int height, bool border);
	void free();

	void addText(std::string string, int x, int y);

	void draw(SDL_Renderer* gRenderer, LTexture* txGameText, LTexture* txSpriteSheet);
};