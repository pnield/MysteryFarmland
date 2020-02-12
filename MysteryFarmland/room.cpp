#include "room.h"

Room::Room(SDL_Rect& size)
{
	this->roomSize.x = size.x;
	this->roomSize.y = size.y;
	this->roomSize.w = size.w;
	this->roomSize.h = size.h;
	connected = false;
}

Room::~Room()
{
	free();
}

void Room::free()
{
	tileArray.erase(tileArray.begin(), tileArray.end());
	roomTileArray.erase(roomTileArray.begin(), roomTileArray.end());
	hallwayArray.erase(hallwayArray.begin(), hallwayArray.end());
	growingPlantArray.erase(growingPlantArray.begin(), growingPlantArray.end());
	actorArray.erase(actorArray.begin(), actorArray.end());
	itemArray.erase(itemArray.begin(), itemArray.end());

	hallwayPathArray.erase(hallwayPathArray.begin(), hallwayPathArray.end());
}

int Room::getRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());//seeds the random number generator
	return std::uniform_int_distribution<int>(min, max)(gen);
}

bool Room::roomCollision(SDL_Rect& otherRoom)
{
	if (roomSize.x - 1 <= otherRoom.x + otherRoom.w &&
		roomSize.x + roomSize.w + 1 >= otherRoom.x &&
		roomSize.y - 1 <= otherRoom.y + otherRoom.h &&
		roomSize.y + roomSize.h + 1 >= otherRoom.y)
	{
		return true;
	}

	return false;
}

bool Room::hallwayCollision(int hallwayX, int hallwayY)
{
	if (roomSize.x <= hallwayX &&
		roomSize.x + roomSize.w >= hallwayX &&
		roomSize.y <= hallwayY &&
		roomSize.y + roomSize.h >= hallwayY)
	{
		return true;
	}

	return false;
}

int Room::getFreeSpace(int& returnX, int& returnY, int& playerX, int& playerY, bool GFSInPlayersRoom)//for adding enemies, items etc
{
	if (GFSInPlayersRoom == false &&
		isInRoomAndDoorway(playerX, playerY) == true)//if you dont want a space in players room and player is in this room
		return -1;//you dont want a space in this room

	std::vector<SDL_Rect> freeSpaceArray;

	for (int x = roomSize.x + 1; x <= roomSize.x + roomSize.w - 1; ++x)
	{
		for (int y = roomSize.y + 1; y <= roomSize.y + roomSize.h - 1; ++y)
		{
			if (playerX == x &&
				playerY == y)
				goto getFreeSpaceContinue;

			for (int i = 0; i < roomTileArray.size(); ++i)
			{
				if (roomTileArray[i]->x == x &&
					roomTileArray[i]->y == y)
					goto getFreeSpaceContinue;
			}

			for (int i = 0; i < growingPlantArray.size(); ++i)
			{
				if (growingPlantArray[i]->exists == true &&
					growingPlantArray[i]->x == x &&
					growingPlantArray[i]->y == y)
					goto getFreeSpaceContinue;
			}

			for (int i = 0; i < actorArray.size(); ++i)
			{
				if (actorArray[i]->status.exists == true &&
					actorArray[i]->x == x &&
					actorArray[i]->y == y)
					goto getFreeSpaceContinue;
			}

			for (int i = 0; i < itemArray.size(); ++i)
			{
				if (itemArray[i]->x == x &&
					itemArray[i]->y == y)
					goto getFreeSpaceContinue;
			}

			for (int i = 0; i < nodeArray.size(); ++i)
			{
				if (x == nodeArray[i]->x + nodeArray[i]->w &&
					y == nodeArray[i]->y + nodeArray[i]->h)//is it blocking the nodes entrance
					goto getFreeSpaceContinue;
			}

			freeSpaceArray.push_back(SDL_Rect{ x, y, 0, 0 });
		
		getFreeSpaceContinue:;
		}
	}

	if (freeSpaceArray.size() > 0)
	{
		int random = 0;
		if (freeSpaceArray.size() > 1)
			random = getRandomInt(0, freeSpaceArray.size() - 1);
		returnX = freeSpaceArray[random].x;
		returnY = freeSpaceArray[random].y;
		return 1;//free space found
	}

	else
		return 0;//no free spaces in room
}

bool Room::isInRoomAndDoorway(int positionX, int positionY)
{
	if (positionX >= roomSize.x &&
		positionX <= roomSize.x + roomSize.w &&
		positionY >= roomSize.y &&
		positionY <= roomSize.y + roomSize.h)
	{
		return true;
	}

	return false;
}

bool Room::isInsideRoom(int positionX, int positionY)
{
	if (positionX > roomSize.x &&
		positionX < roomSize.x + roomSize.w &&
		positionY > roomSize.y &&
		positionY < roomSize.y + roomSize.h)
	{
		return true;
	}

	return false;
}

int Room::getFreeSpaceWithoutPlayer(int& returnX, int& returnY, bool infrontOfDoorway)
{
	std::vector<SDL_Rect> freeSpaceArray;

	for (int x = roomSize.x + 1; x <= roomSize.x + roomSize.w - 1; ++x)
	{
		for (int y = roomSize.y + 1; y <= roomSize.y + roomSize.h - 1; ++y)
		{
			for (int i = 0; i < roomTileArray.size(); ++i)
			{
				if (roomTileArray[i]->x == x &&
					roomTileArray[i]->y == y)
					goto getFreeSpaceWithoutPlayerContinue;
			}

			for (int i = 0; i < growingPlantArray.size(); ++i)
			{
				if (growingPlantArray[i]->exists == true &&
					growingPlantArray[i]->x == x &&
					growingPlantArray[i]->y == y)
					goto getFreeSpaceWithoutPlayerContinue;
			}

			for (int i = 0; i < actorArray.size(); ++i)
			{
				if (actorArray[i]->status.exists == true &&
					actorArray[i]->x == x &&
					actorArray[i]->y == y)
					goto getFreeSpaceWithoutPlayerContinue;
			}

			for (int i = 0; i < itemArray.size(); ++i)
			{
				if (itemArray[i]->x == x &&
					itemArray[i]->y == y)
					goto getFreeSpaceWithoutPlayerContinue;
			}

			if (infrontOfDoorway == false)
			{
				for (int i = 0; i < nodeArray.size(); ++i)
				{
					if (x == nodeArray[i]->x + nodeArray[i]->w &&
						y == nodeArray[i]->y + nodeArray[i]->h)//is it blocking the nodes entrance
						goto getFreeSpaceWithoutPlayerContinue;
				}
			}

			freeSpaceArray.push_back(SDL_Rect{ x, y, 0, 0 });

		getFreeSpaceWithoutPlayerContinue:;
		}
	}

	if (freeSpaceArray.size() > 0)
	{
		int random = getRandomInt(0, freeSpaceArray.size() - 1);
		returnX = freeSpaceArray[random].x;
		returnY = freeSpaceArray[random].y;
		return 1;//free space found
	}

	else
		return 0;//no free spaces in room
}

void Room::drawTiles(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	for (int i = 0; i < hallwayArray.size(); ++i)
	{
		hallwayArray[i]->drawMapPosition(gRenderer, txSpriteSheet, gameView);
	}

	for (int i = 0; i < tileArray.size(); ++i)
	{
		tileArray[i]->drawMapPosition(gRenderer, txSpriteSheet, gameView);
	}

	for (int i = 0; i < roomTileArray.size(); ++i)
	{
		roomTileArray[i]->drawMapPosition(gRenderer, txSpriteSheet, gameView);
	}
}