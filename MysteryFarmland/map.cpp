#include "map.h"

Map::Map()
{
	exists = false;
	mapWidth = 24;
	mapHeight = 24;
	mapTime = 0;
	year = 1;
	season = seasonSpring;
	weatherEffect = noWeather;
}

Map::~Map()
{
	free();
}

void Map::free()//wipes the map to end
{
	roomArray.erase(roomArray.begin(), roomArray.end());
	storedGrowingPlantArray.erase(storedGrowingPlantArray.begin(), storedGrowingPlantArray.end());
	storedBoughtItemArray.erase(storedBoughtItemArray.begin(), storedBoughtItemArray.end());
	shopInventory.erase(shopInventory.begin(), shopInventory.end());
	rainArray.erase(rainArray.begin(), rainArray.end());
}

void Map::nextFloor(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray)//wipes the map so game can regenerate the next level
{
	mapTime = 0;

	rainArray.erase(rainArray.begin(), rainArray.end());

	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			storedGrowingPlantArray.push_back(std::move(roomArray[i]->growingPlantArray[j]));
			roomArray[i]->growingPlantArray.erase(roomArray[i]->growingPlantArray.begin() + j);
			j--;
		}
	}

	roomArray.erase(roomArray.begin(), roomArray.end());

	season++;
	
	if (season > seasonWinter)
	{
		year++;
		season = seasonSpring;
	}

	player->setNextFloor(memoryTextArray);
}

void Map::spawnActor(int& spawnX, int& spawnY, int& roomNumber, int nThSpawnedEnemy)
{
	for (int j = 0; j < percentageSpawnList.size(); ++j)
	{
		if (getRandomInt(1, 100) <= std::get<1>(percentageSpawnList[j]))
		{
			roomArray[roomNumber]->actorArray.push_back(std::make_unique<Actor>(spawnX, spawnY, std::get<0>(percentageSpawnList[j]), true));

			int statusEffectChance = 5;

			if (nThSpawnedEnemy == 2)
				statusEffectChance = 25;
			else if (nThSpawnedEnemy > 2)
				statusEffectChance = 75;

			if (getRandomInt(1, 100) <= statusEffectChance)
			{
				statusEffectList randomStatusEffect = (statusEffectList)getRandomInt(-1, maxStatusEffectListSize - 1);//any status effect including friendly

				if (randomStatusEffect == blockingStatusEffect)
					roomArray[roomNumber]->actorArray[roomArray[roomNumber]->actorArray.size() - 1]->status.blocking = false;
				else if (randomStatusEffect == hastenedStatusEffect)
					roomArray[roomNumber]->actorArray[roomArray[roomNumber]->actorArray.size() - 1]->status.hastened = false;
				else
					roomArray[roomNumber]->actorArray[roomArray[roomNumber]->actorArray.size() - 1]->status.addStatusEffect(randomStatusEffect, 5);
			}

			return;
		}
	}
}

int Map::getRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());//seeds the random number generator
	return std::uniform_int_distribution<int>(min, max)(gen);
}

void Map::create(std::unique_ptr<Player>& player, SDL_Rect& gameView)
{
	setSeasonIcon();

	if (getRandomInt(1, 100) <= 10)
		weatherEffect = rainingWeather;
	else
		weatherEffect = noWeather;

	createRooms();//create rooms and the hallways between them
	createRoomTiles();//create the special tiles that are inside rooms

	int roomSpace;
	int randomRoom = getRandomInt(0, roomArray.size() - 1);
	int randomX;
	int randomY;
	roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, true);
	player->place(randomX, randomY);//place player

	fillSpawnList();//place enemies, items, traps, etc

	for (int i = 0; i < roomArray.size(); ++i)
	{
		int roomEnemyNumber = 1;
		int roomMaxEnemyNumber = numberOfEnemiesInRoom();

		while (roomEnemyNumber <= roomMaxEnemyNumber)
		{
			roomSpace = roomArray[i]->getFreeSpace(randomX, randomY, player->x, player->y, true);

			if (roomSpace == true)
			{
				spawnActor(randomX, randomY, i, roomEnemyNumber);
				roomEnemyNumber++;
			}

			else if (roomSpace <= 0)
			{
				break;
			}
		}
	}
	
	for (int i = 0; i < roomArray.size(); ++i)
	{
		int itemNumber = 1;
		int roomMaxItemNumber = numberOfItemsInRoom();

		while (itemNumber <= roomMaxItemNumber)
		{
			roomSpace = roomArray[i]->getFreeSpace(randomX, randomY, player->x, player->y, true);

			if (roomSpace == true)
			{
				spawnItem(randomX, randomY, i);
				itemNumber++;
			}

			else if (roomSpace <= 0)
				break;
		}
	}

	if (player->equippedItem.get() != nullptr)
	{
		if (player->equippedItem->skill == seedPlusEquipSkill ||
			player->equippedItem->skill == herbPlusEquipSkill)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				if (numberOfItemsInRoom() >= 1)
				{
					roomSpace = roomArray[i]->getFreeSpace(randomX, randomY, player->x, player->y, true);

					if (roomSpace == true)
					{
						if (player->equippedItem->skill == seedPlusEquipSkill)
							roomArray[i]->itemArray.push_back(std::make_unique<Item>(randomX, randomY, seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
						else if (player->equippedItem->skill == herbPlusEquipSkill)
							roomArray[i]->itemArray.push_back(std::make_unique<Item>(randomX, randomY, herbItemType, getRandomInt(0, maxHerbTypeListSize - 1)));
					}
				}
			}
		}
	}
	
	//player->inventory.push_back(std::make_unique<Item>(0, 0, carrotPlantType, 3));//debug
	//player->inventory.push_back(std::make_unique<Item>(0, 0, equipItemType, pitchforkEquipType));//debug
	exists = true;
	player->nextFloor = false;
	player->checkVisible(roomArray, gameView);
	player->recalculatePlantIdentification(roomArray);
	player->recalculateHerbIdentification(roomArray);
}

void Map::setSeasonIcon()
{
	seasonIcon.spriteSheetRect.y = 32;

	if (season == seasonSpring)
		seasonIcon.spriteSheetRect.x = 96;
	else if (season == seasonSummer)
		seasonIcon.spriteSheetRect.x = 104;
	else if (season == seasonFall)
		seasonIcon.spriteSheetRect.x = 112;
	else if (season == seasonWinter)
		seasonIcon.spriteSheetRect.x = 120;
}

void Map::createRooms()
{
	bool regenerate = false;
	tileTypeList skin = tileTypeHouse;//the first made room will be the base and have the skin of a house
	SDL_Rect newRoom;

	for (int i = 0; i < 199; ++i)//make attempts at generating rooms
	{
		if (roomArray.size() > 0)
		{
			newRoom.w = getRandomInt(4, 8);//hardcoded min and max sizes
			newRoom.h = getRandomInt(4, 8);
		}

		else
		{
			newRoom.w = 4;
			newRoom.h = 4;
		}

		newRoom.x = getRandomInt(0, mapWidth - newRoom.w - 1);//the right/down wall is one tile to the right of w/h
		newRoom.y = getRandomInt(0, mapHeight - newRoom.h - 1);

		for (int j = 0; j < roomArray.size(); ++j)
		{
			if (roomArray[j]->roomCollision(newRoom) == true)
			{
				regenerate = true;
				break;
			}
		}

		if (regenerate == true)
		{
			regenerate = false;
			continue;
		}

		generateRoom(newRoom, skin);
	}

	roomArray[0]->connected = true;//the base room, everything connects to it

	if (roomArray.size() > 1)
	{
		createHallways();
	}
}

void Map::generateRoom(SDL_Rect& room, tileTypeList& skin)
{
	std::unique_ptr<Room> tempRoom = std::make_unique<Room>(room);

	for (int i = 0; i <= room.w; ++i)
	{
		for (int j = 0; j <= room.h; ++j)
		{
			if (i == 0)//place wallTypes based on where the wall is
			{
				if (j == 0)
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeUpLeft)));
				}

				else if (j != room.h)
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeLeft)));
				}

				else
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeDownLeft)));
				}
			}

			else if (j == 0 &&
				i != 0 &&
				i != room.w)
			{
				tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeUpMiddle)));
			}

			else if (i == room.w)
			{
				if (j == 0)
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeUpRight)));
				}

				else if (j != room.h)
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeRight)));
				}

				else
				{
					tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeDownRight)));
				}
			}

			else if (j == room.h &&
				i != 0 &&
				i != room.w)
			{
				tempRoom->tileArray.push_back(std::move(std::make_unique<Tile>(room.x + i, room.y + j, skin, wallTypeDownMiddle)));
			}
		}
	}

	roomArray.push_back(std::move(tempRoom));
	skin = tileTypeFence;//every following room is fenced
}

void Map::createHallways()
{
	bool roomsConnected = false;
	int tries = 0;//how many attempts it makes at generation before accepting it cant connect rooms and saying roomsConnected = true
	bool startEndBreak = false;//is it possible to get from startRoom to endRoom
	std::unique_ptr<Tile> startTilePointer;//the actual startTile
	int startRoom;//int for which room in the array it is
	int startTile;//int for which wall tile in that room it starts from
	std::unique_ptr<Tile> endTilePointer;//the actual startTile
	int endRoom;
	int endTile;
	SDL_Rect startTileRect;//the actual tile startRoom's startTile refers to
	SDL_Rect endTileRect;
	SDL_Rect currentStartTileRect;//the actual tiles that walk the hallway and make up hallwayPath
	SDL_Rect currentEndTileRect;
	bool currentStartTileFinish;//finishes if it touches a hallwaypath
	bool currentEndTileFinish;

	std::vector<std::unique_ptr<SDL_Rect>> tempHallwayPathArray;
	bool noDistance = false;//while path hasnt connected

	wallTypeList hallwayWallType;//skin for hallways
	if (season == seasonSpring)
		hallwayWallType = wallTypeHallwaySpring;
	else if (season == seasonSummer)
		hallwayWallType = wallTypeHallwaySummer;
	else if (season == seasonFall)
		hallwayWallType = wallTypeHallwayFall;
	else if (season == seasonWinter)
		hallwayWallType = wallTypeHallwayWinter;

	while (roomsConnected == false)
	{
		roomsConnected = true;

		while (startEndBreak == false)
		{
			goto createHallwaysStartOfGeneration;
		createHallwaysResetGeneration:;
			tempHallwayPathArray.clear();
			if (startTilePointer.get() != nullptr)
			{
				roomArray[startRoom]->tileArray.push_back(std::move(startTilePointer));//put the tile back into the array
				startTilePointer.reset();
			}

			if (endTilePointer.get() != nullptr)
			{
				roomArray[endRoom]->tileArray.push_back(std::move(endTilePointer));
				endTilePointer.reset();
			}

		createHallwaysStartOfGeneration:;
			tries++;
			startEndBreak = true;
			noDistance = false;

			startRoom = getRandomInt(0, roomArray.size() - 1);//randomly pick a room
			startTile = getRandomInt(0, roomArray[startRoom]->tileArray.size() - 1);//randomly pick a wall to start from
			startTilePointer = std::move(roomArray[startRoom]->tileArray[startTile]);//move the actual start tile to the pointer
			roomArray[startRoom]->tileArray.erase(roomArray[startRoom]->tileArray.begin() + startTile);//erase the space in the vector where the tile was

			endRoom = getRandomInt(0, roomArray.size() - 1);
			endTile = getRandomInt(0, roomArray[endRoom]->tileArray.size() - 1);
			endTilePointer = std::move(roomArray[endRoom]->tileArray[endTile]);
			roomArray[endRoom]->tileArray.erase(roomArray[endRoom]->tileArray.begin() + endTile);

			currentStartTileFinish = false;//start tile hasnt touched a different hallway's path and ended
			currentEndTileFinish = false;

			if (startRoom == endRoom)
			{
				goto createHallwaysResetGeneration;
			}

			if (roomArray[startRoom]->connected == true &&
				roomArray[endRoom]->connected == true ||
				roomArray[startRoom]->connected == false &&
				roomArray[endRoom]->connected == false)//if both rooms are already connected or neither of them are then don't connect them
			{
				goto createHallwaysResetGeneration;
			}

			if (startTilePointer->subType != wallTypeUpLeft &&
				startTilePointer->subType != wallTypeUpRight &&
				startTilePointer->subType != wallTypeDownLeft &&
				startTilePointer->subType != wallTypeDownRight)
			{
				if (startTilePointer->subType == wallTypeUpMiddle &&//if the wall is already being used by another doorway, reset
					roomArray[startRoom]->topWallUsed == true ||
					startTilePointer->subType == wallTypeLeft &&
					roomArray[startRoom]->leftWallUsed == true ||
					startTilePointer->subType == wallTypeDownMiddle &&
					roomArray[startRoom]->bottomWallUsed == true ||
					startTilePointer->subType == wallTypeRight &&
					roomArray[startRoom]->rightWallUsed == true)
				{
					goto createHallwaysResetGeneration;
				}

				startTileRect.x = startTilePointer->x;
				startTileRect.y = startTilePointer->y;
				startTileRect.w = 8;
				startTileRect.h = 8;
			}

			else
			{
				goto createHallwaysResetGeneration;
			}

			if (endTilePointer->subType != wallTypeUpLeft &&
				endTilePointer->subType != wallTypeUpRight &&
				endTilePointer->subType != wallTypeDownLeft &&
				endTilePointer->subType != wallTypeDownRight)
			{
				if (endTilePointer->subType == wallTypeUpMiddle &&
					roomArray[endRoom]->topWallUsed == true ||
					endTilePointer->subType == wallTypeLeft &&
					roomArray[endRoom]->leftWallUsed == true ||
					endTilePointer->subType == wallTypeDownMiddle &&
					roomArray[endRoom]->bottomWallUsed == true ||
					endTilePointer->subType == wallTypeRight &&
					roomArray[endRoom]->rightWallUsed == true)
				{
					goto createHallwaysResetGeneration;
				}

				endTileRect.x = endTilePointer->x;
				endTileRect.y = endTilePointer->y;
				endTileRect.w = 8;
				endTileRect.h = 8;
			}

			else
			{
				goto createHallwaysResetGeneration;
			}

			tempHallwayPathArray.clear();
			currentStartTileRect = startTileRect;
			currentEndTileRect = endTileRect;
			tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentStartTileRect)));
			tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentEndTileRect)));

			if (startTilePointer->subType == wallTypeUpMiddle)
			{
				currentStartTileRect.y--;
			}

			else if (startTilePointer->subType == wallTypeLeft)
			{
				currentStartTileRect.x--;
			}

			else if (startTilePointer->subType == wallTypeDownMiddle)
			{
				currentStartTileRect.y++;
			}

			else if (startTilePointer->subType == wallTypeRight)
			{
				currentStartTileRect.x++;
			}

			if (endTilePointer->subType == wallTypeUpMiddle)
			{
				currentEndTileRect.y--;
			}

			else if (endTilePointer->subType == wallTypeLeft)
			{
				currentEndTileRect.x--;
			}

			else if (endTilePointer->subType == wallTypeDownMiddle)
			{
				currentEndTileRect.y++;
			}

			else if (endTilePointer->subType == wallTypeRight)
			{
				currentEndTileRect.x++;
			}

			tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentStartTileRect)));
			tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentEndTileRect)));

			int xDistance = abs(currentStartTileRect.x - currentEndTileRect.x);//absolute distance between the two on the x axis
			int yDistance = abs(currentStartTileRect.y - currentEndTileRect.y);

			while (noDistance == false)//while the path hasn't connected
			{
				if (currentStartTileRect.x < 0 ||
					currentStartTileRect.x > mapWidth ||
					currentStartTileRect.y < 0 ||
					currentStartTileRect.y > mapHeight ||
					currentEndTileRect.x < 0 ||
					currentEndTileRect.x > mapWidth ||
					currentEndTileRect.y < 0 ||
					currentEndTileRect.y > mapHeight)//if pathing goes outside of the maps bounds, start again
				{
					goto createHallwaysResetGeneration;
				}

				if (xDistance >= yDistance &&
					xDistance != 0)
				{
					if (abs(currentStartTileRect.x - currentEndTileRect.x) != 0)
					{
						int newX = 0;

						if (currentStartTileRect.x > currentEndTileRect.x)
						{
							newX = currentStartTileRect.x - 1;
						}

						else if (currentStartTileRect.x < currentEndTileRect.x)
						{
							newX = currentStartTileRect.x + 1;
						}

						for (int i = 0; i < roomArray.size(); ++i)
						{
							for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
							{
								if (roomArray[i]->hallwayCollision(currentStartTileRect.x, currentStartTileRect.y) == true ||
									roomArray[i]->hallwayCollision(newX, currentStartTileRect.y) == true)//if path collides with a room/wall
								{
									goto createHallwaysResetGeneration;
								}
							}

							for (int j = 0; j < roomArray[i]->hallwayPathArray.size(); ++j)
							{
								if (currentStartTileRect.x == roomArray[i]->hallwayPathArray[j]->x &&
									currentStartTileRect.y == roomArray[i]->hallwayPathArray[j]->y)
								{
									currentStartTileFinish = true;
								}
							}
						}

						if (currentStartTileFinish == false)
						{
							if (tempHallwayPathArray.size() > 0 &&
								newX == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->x &&
								currentStartTileRect.y == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->y)//done this way because it would take more code to do the same thing when it comes to checking if tempHallwayPathArray size is > 0
							{
								xDistance = 0;//youre touching the other path
							}

							else//if tempHallwayPathArray is empty or the last entry in it isnt the same as your current position, add them to tempHallwayPathArray
							{
								currentStartTileRect.x = newX;
								tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentStartTileRect)));
							}
						}

						if (abs(currentStartTileRect.x - currentEndTileRect.x) != 0)
						{
							newX = 0;

							if (currentEndTileRect.x > currentStartTileRect.x)
							{
								newX = currentEndTileRect.x - 1;
							}

							else if (currentEndTileRect.x < currentStartTileRect.x)
							{
								newX = currentEndTileRect.x + 1;
							}

							for (int i = 0; i < roomArray.size(); ++i)
							{
								for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
								{
									if (roomArray[i]->hallwayCollision(currentEndTileRect.x, currentEndTileRect.y) == true ||
										roomArray[i]->hallwayCollision(newX, currentEndTileRect.y) == true)
									{
										goto createHallwaysResetGeneration;
									}
								}

								for (int j = 0; j < roomArray[i]->hallwayPathArray.size(); ++j)
								{
									if (currentEndTileRect.x == roomArray[i]->hallwayPathArray[j]->x &&
										currentEndTileRect.y == roomArray[i]->hallwayPathArray[j]->y)
									{
										currentEndTileFinish = true;
									}
								}
							}

							if (currentEndTileFinish == false)
							{
								if (tempHallwayPathArray.size() > 0 &&
									newX == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->x &&
									currentEndTileRect.y == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->y)
								{
									xDistance = 0;
								}

								else
								{
									currentEndTileRect.x = newX;
									tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentEndTileRect)));
								}
							}
						}
					}

					if (abs(currentStartTileRect.x - currentEndTileRect.x) == 0)
					{
						xDistance = 0;
					}
				}

				if (yDistance > xDistance &&
					yDistance != 0)
				{
					if (abs(currentStartTileRect.y - currentEndTileRect.y) != 0)
					{
						int newY = 0;

						if (currentStartTileRect.y > currentEndTileRect.y)
						{
							newY = currentStartTileRect.y - 1;
						}

						else if (currentStartTileRect.y < currentEndTileRect.y)
						{
							newY = currentStartTileRect.y + 1;
						}

						for (int i = 0; i < roomArray.size(); ++i)
						{
							for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
							{
								if (roomArray[i]->hallwayCollision(currentStartTileRect.x, currentStartTileRect.y) == true ||
									roomArray[i]->hallwayCollision(currentStartTileRect.x, newY) == true)
								{
									goto createHallwaysResetGeneration;
								}
							}

							for (int j = 0; j < roomArray[i]->hallwayPathArray.size(); ++j)
							{
								if (currentStartTileRect.x == roomArray[i]->hallwayPathArray[j]->x &&
									currentStartTileRect.y == roomArray[i]->hallwayPathArray[j]->y)
								{
									currentStartTileFinish = true;
								}
							}
						}

						if (currentStartTileFinish == false)
						{
							if (tempHallwayPathArray.size() > 0 &&
								currentStartTileRect.x == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->x &&
								newY == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->y)
							{
								yDistance = 0;
							}

							else
							{
								currentStartTileRect.y = newY;
								tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentStartTileRect)));
							}
						}

						if (abs(currentStartTileRect.y - currentEndTileRect.y) != 0)
						{
							newY = 0;

							if (currentEndTileRect.y > currentStartTileRect.y)
							{
								newY = currentEndTileRect.y - 1;
							}

							else if (currentEndTileRect.y < currentStartTileRect.y)
							{
								newY = currentEndTileRect.y + 1;
							}

							for (int i = 0; i < roomArray.size(); ++i)
							{
								for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
								{
									if (roomArray[i]->hallwayCollision(currentEndTileRect.x, currentEndTileRect.y) == true ||
										roomArray[i]->hallwayCollision(currentEndTileRect.x, newY) == true)
									{
										goto createHallwaysResetGeneration;
									}
								}

								for (int j = 0; j < roomArray[i]->hallwayPathArray.size(); ++j)
								{
									if (currentEndTileRect.x == roomArray[i]->hallwayPathArray[j]->x &&
										currentEndTileRect.y == roomArray[i]->hallwayPathArray[j]->y)
									{
										currentEndTileFinish = true;
									}
								}
							}

							if (currentEndTileFinish == false)
							{
								if (tempHallwayPathArray.size() > 0 &&
									currentEndTileRect.x == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->x &&
									newY == tempHallwayPathArray[tempHallwayPathArray.size() - 1]->y)
								{
									yDistance = 0;
								}

								else
								{
									currentEndTileRect.y = newY;
									tempHallwayPathArray.push_back(std::move(std::make_unique<SDL_Rect>(currentEndTileRect)));
								}
							}
						}
					}

					if (abs(currentStartTileRect.y - currentEndTileRect.y) == 0)
					{
						yDistance = 0;
					}
				}

				if (xDistance == 0 &&
					yDistance == 0 ||
					currentStartTileFinish == true &&
					currentEndTileFinish == true)
				{
					noDistance = true;
					startEndBreak = true;

					for (int i = 0; i < tempHallwayPathArray.size(); ++i)
					{
						roomArray[startRoom]->hallwayPathArray.push_back(std::move(tempHallwayPathArray[i]));
					}

					tempHallwayPathArray.clear();
				}
			}
		}

		fillHallwayPathSides(startRoom, hallwayWallType);

		if (startTilePointer->subType == wallTypeUpMiddle)//sets whichever wall has a doorway as already used
		{
			roomArray[startRoom]->topWallUsed = true;
		}

		else if (startTilePointer->subType == wallTypeLeft)
		{
			roomArray[startRoom]->leftWallUsed = true;
		}

		else if (startTilePointer->subType == wallTypeDownMiddle)
		{
			roomArray[startRoom]->bottomWallUsed = true;
		}

		else if (startTilePointer->subType == wallTypeRight)
		{
			roomArray[startRoom]->rightWallUsed = true;
		}

		if (endTilePointer->subType == wallTypeUpMiddle)
		{
			roomArray[endRoom]->topWallUsed = true;
		}

		else if (endTilePointer->subType == wallTypeLeft)
		{
			roomArray[endRoom]->leftWallUsed = true;
		}

		else if (endTilePointer->subType == wallTypeDownMiddle)
		{
			roomArray[endRoom]->bottomWallUsed = true;
		}

		else if (endTilePointer->subType == wallTypeRight)
		{
			roomArray[endRoom]->rightWallUsed = true;
		}

		roomArray[startRoom]->connected = true;
		roomArray[endRoom]->connected = true;
		startTilePointer.reset();
		endTilePointer.reset();

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->connected == false)
			{
				roomsConnected = false;
				startEndBreak = false;
				break;
			}
		}

		if (tries >= 999)//you've tried enough times to connect rooms, break
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				if (roomArray[i]->connected == false)//erase unconnected rooms
				{
					roomArray.erase(roomArray.begin() + i);
					i--;
				}
			}

			for (int i = 0; i < roomArray.size(); ++i)//fill in any gaps left by unconnected rooms where a hallwayWall should be
			{
				fillHallwayPathSides(i, hallwayWallType);
			}

			roomsConnected = true;//end while loop
		}
	}

	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int r = 0; r < roomArray.size(); ++r)
		{
			for (int h = 0; h < roomArray[r]->hallwayPathArray.size(); ++h)
			{
				if (roomArray[i]->isInRoomAndDoorway(roomArray[r]->hallwayPathArray[h]->x, roomArray[r]->hallwayPathArray[h]->y) == true)
				{
					SDL_Rect node;//the way from the doorway of the room into it
					node.x = roomArray[r]->hallwayPathArray[h]->x;
					node.y = roomArray[r]->hallwayPathArray[h]->y;
					node.w = 0;
					node.h = 0;

					if (node.x == roomArray[i]->roomSize.x)//find "facing" of the doorway by the side of the wall its on and point width/height towards the inside of the room
						node.w = 1;
					else if (node.x == roomArray[i]->roomSize.x + roomArray[i]->roomSize.w)
						node.w = -1;
					else if (node.y == roomArray[i]->roomSize.y)
						node.h = 1;
					else if (node.y == roomArray[i]->roomSize.y + roomArray[i]->roomSize.h)
						node.h = -1;

					roomArray[i]->nodeArray.push_back(std::make_unique<SDL_Rect>(node));
				}
			}
		}
	}
}

void Map::fillHallwayPathSides(int& startRoom, wallTypeList& hallwayWallType)
{
	for (int i = 0; i < roomArray[startRoom]->hallwayPathArray.size(); ++i)
	{
		int upLeft = true;//sides on which to create walls. set to true, false or hallwayTileSkinned
		int up = true;
		int upRight = true;
		int left = true;
		int downLeft = true;
		int down = true;
		int downRight = true;
		int right = true;

		for (int j = 0; j < roomArray.size(); ++j)
		{
			for (int k = 0; k < roomArray[j]->hallwayArray.size(); ++k)
			{
				if (roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)//if a hallway wall collides with hallwayPath, delete the wall
				{
					roomArray[j]->hallwayArray.erase(roomArray[j]->hallwayArray.begin() + k);
					k--;
					continue;
				}
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y - 1) == true)//check which sides of hallwayPath are in a room
			{
				upLeft = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x, roomArray[startRoom]->hallwayPathArray[i]->y - 1) == true)
			{
				up = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y - 1) == true)
			{
				upRight = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y) == true)
			{
				left = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y) == true)
			{
				right = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y + 1) == true)
			{
				downLeft = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x, roomArray[startRoom]->hallwayPathArray[i]->y + 1) == true)
			{
				down = false;
			}

			if (roomArray[j]->hallwayCollision(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y + 1) == true)
			{
				downRight = false;
			}

			for (int k = 0; k < roomArray[j]->tileArray.size(); ++k)//checks if theres already a room wall next to hallwayPath
			{
				if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upLeft = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					up = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upRight = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					left = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					right = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downLeft = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					down = false;
					continue;
				}

				else if (roomArray[j]->tileArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->tileArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downRight = false;
					continue;
				}
			}

			for (int k = 0; k < roomArray[j]->hallwayArray.size(); ++k)//checks if theres already a hallway wall next to hallwayPath
			{
				if (upLeft == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upLeft = false;
					continue;
				}

				else if (up == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					up = false;
					continue;
				}

				else if (upRight == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upRight = false;
					continue;
				}

				else if (left == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					left = false;
					continue;
				}

				else if (right == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					right = false;
					continue;
				}

				else if (downLeft == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downLeft = false;
					continue;
				}

				else if (down == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					down = false;
					continue;
				}

				else if (downRight == true &&
					roomArray[j]->hallwayArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downRight = false;
					continue;
				}
			}

			for (int k = 0; k < roomArray[j]->hallwayPathArray.size(); ++k)//checks if theres already a hallwayPath next to hallwayPath
			{
				if (upLeft == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upLeft = false;
					continue;
				}

				else if (up == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					up = false;
					continue;
				}

				else if (upRight == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y - 1)
				{
					upRight = false;
					continue;
				}

				else if (left == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					left = false;
					continue;
				}

				else if (right == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y)
				{
					right = false;
					continue;
				}

				else if (downLeft == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x - 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downLeft = false;
					continue;
				}

				else if (down == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					down = false;
					continue;
				}

				else if (downRight == true &&
					roomArray[j]->hallwayPathArray[k]->x == roomArray[startRoom]->hallwayPathArray[i]->x + 1 &&
					roomArray[j]->hallwayPathArray[k]->y == roomArray[startRoom]->hallwayPathArray[i]->y + 1)
				{
					downRight = false;
					continue;
				}
			}
		}

		if (upLeft == true)//places hallway walls in the free spaces
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y - 1, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (up == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x, roomArray[startRoom]->hallwayPathArray[i]->y - 1, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (upRight == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y - 1, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (left == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (right == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (downLeft == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x - 1, roomArray[startRoom]->hallwayPathArray[i]->y + 1, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (down == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x, roomArray[startRoom]->hallwayPathArray[i]->y + 1, (tileTypeList)tileTypeHallway, hallwayWallType)));

		if (downRight == true)
			roomArray[startRoom]->hallwayArray.push_back(std::move(std::make_unique<Tile>(roomArray[startRoom]->hallwayPathArray[i]->x + 1, roomArray[startRoom]->hallwayPathArray[i]->y + 1, (tileTypeList)tileTypeHallway, hallwayWallType)));
	}
}

void Map::createRoomTiles()
{
	bool samePlace = true;
	SDL_Rect bed;
	SDL_Rect television;
	int randomRoom;
	int randomX;
	int randomY;

	if (roomArray[0]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
		roomArray[0]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeBed, wallTypeOther));
	if (roomArray[0]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
		roomArray[0]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTV, wallTypeOther));
	fillShopInventory();

	if (roomArray.size() > 1)//spawn shipping container out of house
	{
		randomRoom = getRandomInt(1, roomArray.size() - 1);
		if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
			roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeShippingContainer, wallTypeOther));
	}

	std::vector<int> roomNumberArray;

	for (int i = 0; i < roomArray.size(); ++i)
	{
		roomNumberArray.push_back(i);
		continue;
	}

	while (storedBoughtItemArray.size() > 0 &&
		roomNumberArray.size() > 0)//if there are unplaced bought items
	{
		int randomRoom = getRandomInt(roomNumberArray[0], roomNumberArray[roomNumberArray.size() - 1]);
		int randomX;
		int randomY;

		if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, true) == true)
		{
			storedBoughtItemArray[0]->x = randomX;
			storedBoughtItemArray[0]->y = randomY;
			storedBoughtItemArray[0]->setDrawXY();
			storedBoughtItemArray[0]->pickedUp = false;
			roomArray[randomRoom]->itemArray.push_back(std::move(storedBoughtItemArray[0]));
			storedBoughtItemArray.erase(storedBoughtItemArray.begin() + 0);
		}

		else
			roomNumberArray.erase(roomNumberArray.begin() + randomRoom);
	}

	int trapsPlaced = 0;

	while (trapsPlaced < 2 &&
		roomNumberArray.size() > 0)//place a trap
	{
		int randomRoom = getRandomInt(roomNumberArray[0], roomNumberArray[roomNumberArray.size() - 1]);
		int randomX;
		int randomY;

		if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
		{
			trapsPlaced++;
			roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}

		else
			roomNumberArray.erase(roomNumberArray.begin() + randomRoom);
	}

	for (int i = 0; i < roomNumberArray.size(); ++i)//remove room 0 from roomNumber array
	{
		if (roomNumberArray[i] == 0)
		{
			roomNumberArray.erase(roomNumberArray.begin() + i);
			break;
		}
	}

	while (storedGrowingPlantArray.size() > 0 &&
		roomNumberArray.size() > 0)//if there are unplaced growing plants and there are free rooms
	{
		int randomRoom = getRandomInt(roomNumberArray[0], roomNumberArray[roomNumberArray.size() - 1]);
		int randomX;
		int randomY;

		if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
		{
			storedGrowingPlantArray[0]->x = randomX;
			storedGrowingPlantArray[0]->y = randomY;
			storedGrowingPlantArray[0]->setDrawXY();

			if (storedGrowingPlantArray[0]->watered == false ||
				season == seasonSpring)
				storedGrowingPlantArray[0]->wither();

			storedGrowingPlantArray[0]->watered = false;//growingPlants can be visited to raise their growth every new floor
			storedGrowingPlantArray[0]->seasonGrowth = 0;
			roomArray[randomRoom]->growingPlantArray.push_back(std::move(storedGrowingPlantArray[0]));
			storedGrowingPlantArray.erase(storedGrowingPlantArray.begin() + 0);
		}

		else
			roomNumberArray.erase(roomNumberArray.begin() + randomRoom);//if the room is full, erase it and try again
	}
}

void Map::fillShopInventory()
{
	shopInventory.erase(shopInventory.begin(), shopInventory.end());

	for (int i = 0; i < 4; ++i)
	{
		shopInventory.push_back(std::make_unique<Item>(seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
	}

	shopInventory.push_back(std::make_unique<Item>(equipItemType, getRandomInt(0, maxEquipTypeListSize - 1)));
}

void Map::fillSpawnList()
{
	percentageSpawnList.clear();

	if (year == 1)
	{
		if (season <= seasonSummer)//easy enemies
		{
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(snailActorType, 10));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(squirrelActorType, 25));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(chickActorType, 100));
		}

		else//slightly stronger enemies
		{
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(crowActorType, 10));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(mouseActorType, 25));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(chickenActorType, 100));
		}
	}

	else if (year == 2)
	{
		if (season <= seasonSummer)
		{
			
		}

		else
		{
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(snailActorType, 10));//turtle?
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(chickenActorType, 25));//skunk?
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(cowActorType, 100));
		}
	}

	else if (year == 3)
	{
		if (season <= seasonSummer)//cats wake strong enemy and they all attack you. this could be a stronger echo of lvl 1 with turtle and stronger squirrel and rooster?
		{
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(catActorType, 10));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(kittenActorType, 25));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(chickenActorType, 100));
		}

		else//strong echo of past enemies
		{
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(vultureActorType, 10));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(ratActorType, 25));
			percentageSpawnList.push_back(std::tuple<actorTypeList, int>(roosterActorType, 100));
		}
	}
}

int Map::numberOfEnemiesInRoom()
{
	int randomInt = getRandomInt(1, 100);

	if (randomInt <= 20)
		return 0;
	else if (randomInt <= 70)
		return 1;
	else if (randomInt <= 90)
		return 2;
	else
		return 3;
}

int Map::numberOfItemsInRoom()
{
	int randomInt = getRandomInt(1, 100);

	if (season == seasonWinter)
	{
		if (randomInt <= 90)
			return 0;
		if (randomInt <= 95)
			return 1;
		if (randomInt <= 99)
			return 2;
		else
			return 3;
	}

	if (randomInt <= 60)
		return 0;
	else if (randomInt <= 75)
		return 1;
	else if (randomInt <= 99)
		return 2;
	else
		return 3;
}

void Map::spawnItem(int& spawnX, int& spawnY, int& roomNumber)
{
	int spawnChance = getRandomInt(1, 100);

	if (season == seasonSpring)
	{
		if (spawnChance <= 75)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
		else if (spawnChance <= 90)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, herbItemType, getRandomInt(0, maxHerbTypeListSize - 1)));
		else
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, equipItemType, getRandomInt(0, maxEquipTypeListSize - 1)));
	}

	else if (season == seasonFall)
	{
		if (spawnChance <= 35)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
		else if (spawnChance <= 90)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, herbItemType, getRandomInt(0, maxHerbTypeListSize - 1)));
		else
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, equipItemType, getRandomInt(0, maxEquipTypeListSize - 1)));
	}

	else
	{
		if (spawnChance <= 55)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
		else if (spawnChance <= 90)
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, herbItemType, getRandomInt(0, maxHerbTypeListSize - 1)));
		else
			roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(spawnX, spawnY, equipItemType, getRandomInt(0, maxEquipTypeListSize - 1)));
	}
}

void Map::run(ResourceManager* resourceManager, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	if (player.get() != nullptr)
	{
		rainAnimate(player, gameView);

		if (player->playerTurn == true ||
			player->playerTurn == secondTurn)
			player->update(roomArray, mapWidth, mapHeight, season, memoryTextArray, shopInventory, storedBoughtItemArray, gameView);

		if (player->status.exists == true)
		{
			if (player->playerTurn == false ||
				player->playerTurn == waitingForSecondTurn)
			{
				if (player->status.stopTime == false)
				{
					stormCountdown(player, memoryTextArray);
					rainWaterGrowingPlants();

					for (int i = 0; i < roomArray.size(); ++i)
					{
						for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
						{
							if (roomArray[i]->actorArray[j]->status.exists == false)//dead actors drop heldItem and are erased
							{
								roomArray[i]->actorArray[j]->dropItem(roomArray);
								roomArray[i]->actorArray.erase(roomArray[i]->actorArray.begin() + j);
								j--;
								continue;
							}
						}

						for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
						{
							roomArray[i]->actorArray[j]->update(player, roomArray, memoryTextArray);
						}

						for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
						{
							roomArray[i]->growingPlantArray[j]->grow(season);

							if (roomArray[i]->growingPlantArray[j]->exists == false)//dead plants are erased
							{
								roomArray[i]->growingPlantArray.erase(roomArray[i]->growingPlantArray.begin() + j);
								j--;
								continue;
							}
						}
					}
				}

				player->checkVisible(roomArray, gameView);
			}

			if (player->playerTurn == false ||
				player->playerTurn == afterSecondTurn)//if players not hastened, do your next turn. if players hastened, wait for his turn first
			{
				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.hastened >= true)
							roomArray[i]->actorArray[j]->update(player, roomArray, memoryTextArray);//hastened enemies get a second turn

						if (roomArray[i]->actorArray[j]->status.exists == false)//dead actors drop heldItem and are erased
						{
							roomArray[i]->actorArray[j]->dropItem(roomArray);
							roomArray[i]->actorArray.erase(roomArray[i]->actorArray.begin() + j);
							j--;
							continue;
						}
					}
				}

				player->checkVisible(roomArray, gameView);
			}

			if (player->playerTurn == waitingForSecondTurn)//set to second turn and next loop this function will let player take his second turn before actors second turn
				player->playerTurn = secondTurn;
		}

		if (player->playerTurn == false ||
			player->playerTurn == afterSecondTurn)
			player->playerTurn = true;
	}
}

void Map::rainAnimate(std::unique_ptr<Player>& player, SDL_Rect& gameView)
{
	if (weatherEffect == rainingWeather ||
		weatherEffect == stormingWeather)
	{
		int rainSpeed = 2;

		if (weatherEffect == stormingWeather)
			rainSpeed = 5;

		if (rainArray.size() == 0)
		{
			for (int i = 0; i < 16; ++i)
			{
				if(season != seasonWinter)
					rainArray.push_back(std::make_unique<BaseThing>(getRandomInt(i * 8, i * 8 + 4), getRandomInt(0, 128), 40, 40));
				else
					rainArray.push_back(std::make_unique<BaseThing>(getRandomInt(i * 8, i * 8 + 4), getRandomInt(0, 128), 96, 40));
			}
		}

		if (player->status.stopTime == false)
		{
			for (int i = 0; i < rainArray.size(); ++i)
			{
				rainArray[i]->y += rainSpeed;

				if (rainArray[i]->y >= 128)
				{
					rainArray[i]->x = getRandomInt(i * 8, i * 8 + 4);
					rainArray[i]->y = getRandomInt(-20, 0);
				}
			}
		}
	}
}

void Map::stormCountdown(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	mapTime++;

	if (mapTime == 300)
	{
		if(season != seasonWinter)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I hear distant thunder...")));
		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I hear distant snowfall...")));
	}

	else if (mapTime == 350)
	{
		if (season != seasonWinter)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("A terrible storm appears...")));
		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("A terrible snowstorm appears...")));

		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I'd better get to bed.")));
		weatherEffect = stormingWeather;
	}

	else if (mapTime == 400)
	{
		if (season != seasonWinter)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I was struck by lightning!")));
		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I was buried in snow!")));

		player->status.exists = false;
	}
}

void Map::rainWaterGrowingPlants()
{
	if (weatherEffect == rainingWeather ||
		weatherEffect == stormingWeather)
	{
		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->alive == true &&
					roomArray[i]->growingPlantArray[j]->watered == false)
					roomArray[i]->growingPlantArray[j]->careForMe(true);
			}
		}
	}
}