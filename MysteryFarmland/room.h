#pragma once

#include <vector>
#include <memory>

#include "tile.h"
#include "actor.h"
#include "growingPlant.h"

class Room
{
public:
	SDL_Rect roomSize;
	bool connected;//is it connected to the base room
	bool topWallUsed;//if this wall has a doorway on it already
	bool leftWallUsed;
	bool bottomWallUsed;
	bool rightWallUsed;
	std::vector<std::unique_ptr<Tile>> tileArray;
	std::vector<std::unique_ptr<Tile>> roomTileArray;//tiles inside room, bed, tv, traps, etc
	std::vector<std::unique_ptr<Tile>> hallwayArray;
	std::vector<std::unique_ptr<GrowingPlant>> growingPlantArray;
	std::vector<std::unique_ptr<Actor>> actorArray;
	std::vector<std::unique_ptr<Item>> itemArray;

	std::vector<std::unique_ptr<SDL_Rect>> hallwayPathArray;//the hallway path used for filling in hallway walls during generation and after erasing unconnected rooms. could be useful later
	std::vector<std::unique_ptr<SDL_Rect>> nodeArray;//nodes at doorways that allow to travel to other rooms

	Room(SDL_Rect& roomSize);
	~Room();

	void free();

	int getRandomInt(int min, int max);

	bool roomCollision(SDL_Rect& otherRoom);
	bool hallwayCollision(int hallwayX, int hallwayY);//hallway walls check if they're going to be placed inside of an existing room
	int getFreeSpace(int& returnX, int& returnY, int& playerX, int& playerY, bool GFSInPlayersRoom);//check room for free spaces with no player, actor, object, etc
	bool isInRoomAndDoorway(int positionX, int positionY);
	bool isInsideRoom(int positionX, int positionY);//if in room but not its doorway
	int getFreeSpaceWithoutPlayer(int& returnX, int& returnY, bool infrontOfDoorway);

	void drawTiles(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);
};