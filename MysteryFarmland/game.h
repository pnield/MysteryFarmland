#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>

#include "resourceManager.h"
#include "player.h"
#include "menu.h"
#include "map.h"

class Game
{
public:
	SDL_Window* gWindow = NULL;

	SDL_Renderer* gRenderer = NULL;

	ResourceManager resourceManager;
	std::unique_ptr<Player> player;
	bool playerCreated;
	Map map;

	SDL_Color textColor;//palette's white color

	Menu topMenu;//menu at the top of screen
	Menu playerMenu;//interactable game, inventory and floor menus
	Menu itemMenu;//menu for using items in inventory. should probably make playerMenu an array of menus later
	Menu equipMenu;//menu that shows crop abilities and hunger when playerMenu is open
	Menu memoryMenu;//menu that shows text of past events

	std::vector<std::unique_ptr<std::string>> memoryTextArray;//array of memory event strings

	SDL_Rect gameView;//the size of the screen and hopefully the x and y of the camera
	int screenMultiplier;//what multiplication the screen is drawn at
	int screenWidth;
	int screenHeight;

	bool playerMenuCreated;
	bool itemMenuCreated;
	bool inventoryMenuCreated;
	bool floorMenuCreated;
	bool shopMenuCreated;

	int memoryOfMapSeason;
	int memoryOfPlayerHealth;
	int memoryOfPlayerMaxHealth;
	int memoryOfPlayerMoney;

	int memoryOfInventorySize;

	int memoryOfMemoryMenuSize;
	int memoryMenuTimer;

	int upLeftKey;
	int upRightKey;
	int downLeftKey;
	int downRightKey;
	int upKey;
	int downKey;
	int leftKey;
	int rightKey;
	int attackKey;
	int facingKey;
	int menuKey;
	int diagonalKey;

	double moveTimer;//timer for repeating move actions

	Game();
	~Game();

	int startAndRun();
	bool initSDL();
	bool loadMedia();
	int gameLoop();

	void update();

	void readInput();

	void menuState();//handles menus
	void createMemoryMenu();
	void setMenusFalse();
	void createPlayerMenu();
	void createInventoryMenu();
	void createItemMenu();
	void createFloorMenu();
	void createShopMenu();

	void createTopMenu();

	void createEquipMenu();

	void draw();
	void drawLayers();

	void close();
};