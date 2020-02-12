#pragma once

#include "baseThing.h"

enum tileTypeList { tileTypeHouse, tileTypeFence, tileTypeHallway, tileTypeTrap, tileTypeBed, tileTypeTV, tileTypeShippingContainer };
enum wallTypeList { wallTypeUpLeft, wallTypeUpMiddle, wallTypeUpRight, wallTypeLeft, wallTypeDownLeft, wallTypeDownMiddle, wallTypeDownRight, wallTypeRight, wallTypeHallwaySpring, wallTypeHallwaySummer, wallTypeHallwayFall, wallTypeHallwayWinter, wallTypeOther };
enum trapTypeList { gasTrapType, dropTrapType, trapTrapType, forgetTrapType, sleepTrapType, hungerTrapType, wakeTrapType, maxTrapTypeListSize };

class Tile : public BaseThing
{
public:
	tileTypeList tileType;
	int subType;
	bool discovered;//has the trap been discovered

	Tile(int x, int y, tileTypeList tileType, int subType);
	~Tile();

	void tileSprite();
};