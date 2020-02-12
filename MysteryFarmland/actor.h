#pragma once

#include "baseLifeform.h"
#include "player.h"

class Room;
class GrowingPlant;

enum actorSpecialEnumList { roomUninitialized = -1, nodeUninitialized = -1, targettingPlayer = 1, targettingEnemyActor = 2 };
enum actorTypeList { evolveTypeNone = -1, chickActorType, chickenActorType, roosterActorType, squirrelActorType, snailActorType, mouseActorType, ratActorType, crowActorType, vultureActorType, kittenActorType, catActorType, hyenaActorType, cowActorType, bullActorType, maxActorTypeListSize };
enum actorStyleList { normalActorStyle, thiefActorStyle, huntSeedsActorStyle, huntCropsActorStyle, enemyWakerActorStyle };

class Actor : public BaseLifeform
{
public:
	bool rolledForActive;//have I rolled to wake up
	actorTypeList actorType;
	actorStyleList actorStyle;
	actorTypeList evolveType;
	actorTypeList devolveType;
	int canSeeEnemy;
	int enemyActorRoomArrayNumber;
	int enemyActorActorArrayNumber;
	int targetRoom;//which room the targetNode is in. only path to the node if im in this room
	int targetNode;//which doorways node im targetting

	std::unique_ptr<Item> heldItem;

	Actor(int x, int y, actorTypeList actorType, bool isAsleep);
	~Actor();

	void free();

	void setType();//set spriteSheetRect and variables based on its current actorType
	void setSpawnHealth(int number);

	bool move(int moveX, int moveY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	bool moveDiagonal(int moveX, int moveY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//diagonal pathing from actorPathing
	bool moveAroundACorner(int moveX, int moveY, int cornerBlockedTileX, int cornerBlockedTileY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//checks if it can turn around a right corner
	//bool targetNodeIsPlayersNode(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray);
	void drunkAct(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void attackPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void attackActor(std::unique_ptr<Actor>& targetActor, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void attackGrowingPlant(int attackX, int attackY, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void attackingDamage(int& damageDealt, int& rumbleDealt);
	bool ifInRangeDiagonalBlocked(int targetX, int targetY, std::vector<std::unique_ptr<Room>>& roomArray);
	void powerUp(std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void dropItem(std::vector<std::unique_ptr<Room>>& roomArray);
	void setTargetToPlayersNode(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray);

	bool isPlayerInRange(bool attack, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray);//is player in a 1x1 range
	bool isEnemyActorInRange(bool attack, bool honorable, std::unique_ptr<Actor>& targetActor, std::vector<std::unique_ptr<Room>>& roomArray);
	bool isGrowingPlantInRange(bool attack, std::unique_ptr<GrowingPlant>& targetGrowingPlant, std::vector<std::unique_ptr<Room>>& roomArray);

	void update(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void checkStatus();

	bool floorInteract(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void trapInteract(int& roomArrayNumber, int& roomTileArrayNumber, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void actorStyleAction(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//what type of actor = what type of actions
		bool isEnemyActorNearby(bool enemyInDoorway, std::vector<std::unique_ptr<Room>>& roomArray);
		bool inPlayerRoom(bool playerInDoorway, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray);//is sharing a room with player
	void actorPathing(bool turnCorners, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//all pathing actions
	void stealFromPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void huntSeeds(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//move to seeds in room, else roam
	void huntGrowingPlants(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//move to growingPlants in room, else roam
	void moveToEnemyActor(std::unique_ptr<Actor>& targetActor, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void moveToPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void findRoomToNode(bool followPlayer, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//find farthest node in this room
		void moveToNode(int& roomNumber, bool followPlayer, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void fleeToNode(int& roomNumber, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);//find the node farthest from player who is in the room
	void friendlyWander(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void timeDownStatus();

	void draw(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);
};