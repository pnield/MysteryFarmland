#include "menu.h"

Menu::Menu()
{
	menuRect.x = 0;
	menuRect.y = 0;
	menuRect.w = 0;
	menuRect.h = 0;
	border = false;
}

Menu::~Menu()
{
	free();
}

void Menu::create(int x, int y, int width, int height, bool border)
{
	free();

	menuRect.x = x;
	menuRect.y = y;
	menuRect.w = width;
	menuRect.h = height;
	this->border = border;

	textboxPtr = std::make_unique<Textbox>(menuRect.x, menuRect.y, menuRect.w, menuRect.h);
}

void Menu::free()
{
	textboxPtr.reset();
	iconArray.erase(iconArray.begin(), iconArray.end());
	textArray.erase(textArray.begin(), textArray.end());
}

void Menu::addText(std::string string, int x, int y)
{
	std::unique_ptr<std::vector<std::unique_ptr<Text>>> tempArray = std::make_unique<std::vector<std::unique_ptr<Text>>>();

	for (int i = 0; i < string.size(); ++i)
	{
		tempArray->push_back(std::move(std::make_unique<Text>(&string[i], x + (i * 4), y)));
	}

	textArray.push_back(std::move(tempArray));
}

void Menu::draw(SDL_Renderer* gRenderer, LTexture* txGameText, LTexture* txSpriteSheet)
{
	if (textboxPtr.get() != nullptr)
	{
		textboxPtr->draw(gRenderer);

		if (border == true)
		{
			textboxPtr->drawBorder(gRenderer);
		}
	}

	for (int i = 0; i < textArray.size(); ++i)
	{
		for (int j = 0; j < textArray[i]->size(); ++j)
		{
			(*textArray[i])[j]->draw(gRenderer, txGameText);
		}
	}

	for (int j = 0; j < iconArray.size(); ++j)
	{
		iconArray[j]->drawRealPosition(gRenderer, txSpriteSheet);
	}
}