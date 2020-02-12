#include "tile.h"

Tile::Tile(int x, int y, tileTypeList tileType, int subType)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();
	this->tileType = tileType;
	this->subType = subType;
	discovered = false;
	tileSprite();
}

Tile::~Tile()
{

}

void Tile::tileSprite()
{
	if (tileType == tileTypeHouse)
	{
		spriteSheetRect.x = 104;
		spriteSheetRect.y = 8;
	}

	else if (tileType == tileTypeFence)
	{
		if (subType == wallTypeUpLeft)
		{
			spriteSheetRect.x = 96;
			spriteSheetRect.y = 0;
		}

		else if (subType == wallTypeUpMiddle)
		{
			spriteSheetRect.x = 104;
			spriteSheetRect.y = 0;
		}

		else if (subType == wallTypeUpRight)
		{
			spriteSheetRect.x = 112;
			spriteSheetRect.y = 0;
		}

		else if (subType == wallTypeLeft)
		{
			spriteSheetRect.x = 96;
			spriteSheetRect.y = 8;
		}

		else if (subType == wallTypeRight)
		{
			spriteSheetRect.x = 112;
			spriteSheetRect.y = 8;
		}

		else if (subType == wallTypeDownLeft)
		{
			spriteSheetRect.x = 96;
			spriteSheetRect.y = 16;
		}

		else if (subType == wallTypeDownMiddle)
		{
			spriteSheetRect.x = 104;
			spriteSheetRect.y = 16;
		}

		else if (subType == wallTypeDownRight)
		{
			spriteSheetRect.x = 112;
			spriteSheetRect.y = 16;
		}
	}

	else if (tileType == tileTypeHallway)
	{
		if (subType == wallTypeHallwaySpring)
		{
			spriteSheetRect.x = 96;
			spriteSheetRect.y = 24;
		}

		else if (subType == wallTypeHallwaySummer)
		{
			spriteSheetRect.x = 104;
			spriteSheetRect.y = 24;
		}

		else if (subType == wallTypeHallwayFall)
		{
			spriteSheetRect.x = 112;
			spriteSheetRect.y = 24;
		}

		else if (subType == wallTypeHallwayWinter)
		{
			spriteSheetRect.x = 120;
			spriteSheetRect.y = 24;
		}
	}

	else if (tileType == tileTypeTrap)
	{
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 64;
	}

	else if (tileType == tileTypeBed)
	{
		spriteSheetRect.x = 120;
		spriteSheetRect.y = 0;
	}

	else if (tileType == tileTypeTV)
	{
		spriteSheetRect.x = 120;
		spriteSheetRect.y = 8;
	}

	else if (tileType == tileTypeShippingContainer)
	{
		spriteSheetRect.x = 120;
		spriteSheetRect.y = 16;
	}
}