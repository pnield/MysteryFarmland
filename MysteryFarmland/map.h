#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <sstream>
#include <tuple>

#include "resourceManager.h"
#include "player.h"
#include "room.h"
#include "text.h"

class Map
{
public:
	bool exists;
	int mapWidth;
	int mapHeight;
	int mapTime;//counter of how many turns have passed

	int year;//every fourth floor
	int season;//floor
	weatherList weatherEffect;
	BaseThing seasonIcon;
	std::vector<std::tuple<actorTypeList, int>> percentageSpawnList;//on this floor: what enemies spawn/what chance they have to spawn/will they spawn crippled with a status effect

	std::vector<std::unique_ptr<Room>> roomArray;
	std::vector<std::unique_ptr<GrowingPlant>> storedGrowingPlantArray;//growing plants held as they move to the next level
	std::vector<std::unique_ptr<Item>> storedBoughtItemArray;
	std::vector<std::unique_ptr<Item>> shopInventory;

	std::vector<std::unique_ptr<BaseThing>> rainArray;

	Map();
	~Map();

	void free();
	void nextFloor(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void spawnActor(int& spawnX, int& spawnY, int& roomNumber, int nThSpawnedEnemy);

	int getRandomInt(int min, int max);

	void create(std::unique_ptr<Player>& player, SDL_Rect& gameView);//create the map
	void setSeasonIcon();
	void createRooms();//create rooms and the hallways between them
	void generateRoom(SDL_Rect& room, tileTypeList& skin);//create the tiles for a room
	void createHallways();
	void fillHallwayPathSides(int& startRoom, wallTypeList& hallwayWallType);//fill the non-used sides of the hallway path with walls
	void createRoomTiles();//create the special tiles that are inside rooms
	void fillShopInventory();
	void fillSpawnList();//set the level to spawn that level's enemies
	int numberOfEnemiesInRoom();
	int numberOfItemsInRoom();
	void spawnItem(int& spawnX, int& spawnY, int& roomNumber);

	void run(ResourceManager* resourceManager, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
		void rainAnimate(std::unique_ptr<Player>& player, SDL_Rect& gameView);
		void stormCountdown(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void rainWaterGrowingPlants();
};