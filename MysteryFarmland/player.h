#pragma once

#include <vector>

#include "baseLifeform.h"
#include "text.h"
#include "item.h"

class Room;

enum playerSpecialEnumList { keyHeld = 2, waitingForSecondTurn = 2, secondTurn = 3, afterSecondTurn = 4 };
enum gameStateList { gameStateMainMenu, gameStateIngame, gameStateFacing, gameStatePlayerMenu, gameStateInventory, gameStateShipping, gameStateItemMenu, gameStateIdentifyItem, gameStateDestroyItem, gameStateFloorMenu, gameStateShopMenu, gameStateQuit };
enum playerMenuList { playerMenuInventory, playerMenuUse, playerMenuFloor, playerMenuQuit, maxPlayerMenuListSize };
enum itemMenuList { itemMenuUse, itemMenuThrow, itemMenuDrop, maxItemMenuListSize };
enum floorMenuList { floorMenuEquip, floorMenuIgnore };

enum weatherList { noWeather, rainingWeather, stormingWeather };

enum TVChannelList { noChannel, shoppingChannel, adviceChannel };

class Player : public BaseLifeform
{
public:
	SDL_Rect facingArrowSpriteSheetRect;

	std::unique_ptr<Item> equippedItem;
	std::vector<std::unique_ptr<Item>> inventory;

	bool nextFloor;
	int playerTurn;//false, true or secondTurn when fleeing, etc.
	bool statusChecked;//have i checked if i'm dead, applied turn based status effects, timed down status effects, etc. this turn
	gameStateList gameState;//in gameplay, in the main menu, in hudMenu, etc.
	int menuState;//if gamestate has a menu, this cycles through it
	int itemMenuState;
	bool justMoved;//if true, interacts with things you step on
	bool standingOnObject;//if standing on object can use floorMenu with it
	bool preciseStep;//if stepping in such a way to move through enemies and not pick up items
	bool attackAnimating;//is attacking, to tell the gameView not to follow during this

	int healthRegenerationMultiplier;
	double healthRegeneration;
	int hungerSatiety;
	int satietyDegenMultiplier;
	double satietyDegen;
	int maxExp;
	int money;
	int tempAttackPower;//how much atk is given by temporary buffs like vitamin a
	int tempDefensePower;
	bool trueMansWorld;//if i don't attack perma-sleeping enemies

	std::vector<plantTypeList> identifiedPlants;
	std::vector<herbTypeList> identifiedHerbs;

	int upLeftKey;
	int upRightKey;
	int downLeftKey;
	int downRightKey;
	int upKey;
	int downKey;
	int leftKey;
	int rightKey;
	int attackKey;//attack/accept
	int facingKey;//facing/close
	int menuKey;//open menu
	int diagonalKey;//diagonal movement key

	Player();
	Player(int x, int y);
	~Player();

	void free();

	void create();
	void place(int& x, int& y);

	void setNextFloor(std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void setFacing(int facingX, int facingY, bool animateAttack);//sets facing towards given position
	void targetFacing(int& targetX, int& targetY, bool animateAttack);//returns a +/- 1 value towards faced direction

	void preciseMovementInput(std::vector<std::unique_ptr<Room>>& roomArray);
	void movementInput(bool isPreciseStepping, std::vector<std::unique_ptr<Room>>& roomArray);
	void onMove(bool isPreciseStepping);
	bool takeDefendableDamage(int& damageTaken);
	void takeTrueDamage(int damageTaken);
	void eatFood(int nutritionGained, std::string name, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void removeMenuStateFromInventory();
	void careForPlant(bool watering, int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
	void recalculatePlantIdentification(std::vector<std::unique_ptr<Room>>& roomArray);
	void recalculateHerbIdentification(std::vector<std::unique_ptr<Room>>& roomArray);

	void update(std::vector<std::unique_ptr<Room>>& roomArray, int& mapWidth, int& mapHeight, int& season, std::vector<std::unique_ptr<std::string>>& memoryTextArray, std::vector<std::unique_ptr<Item>>& shopInventory, std::vector<std::unique_ptr<Item>>& storedBoughtItemArray, SDL_Rect& gameView);

	void checkStatus(std::vector<std::unique_ptr<std::string>>& memoryTextArray);//once per turn. check if i'm dead, apply and count down status effects, etc.
	
	void checkVisible(std::vector<std::unique_ptr<Room>>& roomArray, SDL_Rect& gameView);
		void resetVisible(std::vector<std::unique_ptr<Room>>& roomArray);

	void handleInput(std::vector<std::unique_ptr<Room>>& roomArray, int& mapWidth, int& mapHeight, int& season, std::vector<std::unique_ptr<std::string>>& memoryTextArray, std::vector<std::unique_ptr<Item>>& shopInventory, std::vector<std::unique_ptr<Item>>& storedBoughtItemArray, SDL_Rect& gameView);
		void drunkWalk();//if confused, ingame and movement keys input, scramble them and a random key is pressed instead
		void dKeyDiagonalInput();
		void findAttackTarget(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
			void attackRoomTile(int& roomNumber, int& roomTileNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void attackGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void attackEnemy(int& roomNumber, int& enemyNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
		bool quickWait(std::vector<std::unique_ptr<Room>>& roomArray);
		void handleFacing();
		void useSlottedCropAbility(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
			void cucumberMovesActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray);
		void identifyMenuState(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void destroyMenuState(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void useItem(int& season, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
			int getFreePlantingSpace(int& returnX, int& returnY, int& currentRoom, int& season, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void useEatenCropAbility(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView);
			void rememberHerbPower(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void forgetHerbPower(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void throwItem(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void throwHitsGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
				void throwCropAffectsGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
			void throwHitsActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
				void thrownCropAffectsActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void floorInteract(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void objectInteract(int& roomArrayNumber, int& itemArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void equipInteract(int& roomArrayNumber, int& itemArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);
		void trapInteract(int& roomArrayNumber, int& roomTileArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray);

	void endTurn();
		void timeDownStatus();

	void draw(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);
		void setGameViewAndDrawXY(SDL_Rect& gameView);
		void setFacingSprite();
		void drawFacingArrow(SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
};