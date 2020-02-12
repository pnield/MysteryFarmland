#include "game.h"

Game::Game()
{
	gameView.x = 0;
	gameView.y = 0;
	gameView.w = 128;
	gameView.h = 128;
	screenMultiplier = 4;
	screenWidth = gameView.w * screenMultiplier;
	screenHeight = gameView.h * screenMultiplier;

	playerCreated = false;

	textColor.r = 255;
	textColor.g = 241;
	textColor.b = 232;

	setMenusFalse();

	memoryOfMapSeason = -1;//uninitialized
	memoryOfPlayerHealth = -1;
	memoryOfPlayerMaxHealth = -1;
	memoryOfPlayerMoney = -1;

	memoryOfInventorySize = -1;

	memoryOfMemoryMenuSize = 0;
	memoryMenuTimer = INT_MAX;

	upLeftKey = false;
	upRightKey = false;
	downLeftKey = false;
	downRightKey = false;
	upKey = false;
	downKey = false;
	leftKey = false;
	rightKey = false;
	attackKey = false;
	menuKey = false;
	facingKey = false;
	diagonalKey = false;
}

Game::~Game()
{
	close();
}

bool Game::initSDL()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	else
	{
		//Create window
		gWindow = SDL_CreateWindow("Mystery Farmland", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOWEVENT_HIDDEN);

		//SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}

		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}

			else
			{
				SDL_RenderSetLogicalSize(gRenderer, gameView.w, gameView.h);
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
				SDL_RenderClear(gRenderer);
				SDL_RenderPresent(gRenderer);

				SDL_ShowWindow(gWindow);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool Game::loadMedia()
{
	bool success = false;

	if (resourceManager.load(gRenderer))
	{
		success = true;
	}

	return success;
}

int Game::startAndRun()
{
	if (initSDL() == false)
	{
		printf("Failed to initialize!\n");
	}

	else
	{
		if (loadMedia() == false)
		{
			printf("Failed to load media!\n");
		}

		else
		{
			player = std::make_unique<Player>();
			map.create(player, gameView);
			player->setGameViewAndDrawXY(gameView);
			gameLoop();
		}
	}

	close();

	return 0;
}

int Game::gameLoop()
{
	bool quit = false;
	SDL_Event SDLevent;

	const double deltaTime = 1000.f / 60.f;//time passed between each update
	double newTime;
	double frameTime;//time passed since last loop
	double oldTime = SDL_GetTicks();
	double accumulatedFrameTime = 0;//counter of time passed

	while (!quit)
	{
		newTime = SDL_GetTicks();
		frameTime = newTime - oldTime;
		oldTime = newTime;
		accumulatedFrameTime += frameTime;

		if (accumulatedFrameTime >= deltaTime)
		{
			moveTimer += accumulatedFrameTime;
			accumulatedFrameTime = 0;

			SDL_PollEvent(&SDLevent);

			update();

			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderClear(gRenderer);
			draw();
			SDL_RenderPresent(gRenderer);

			if (SDLevent.type == SDL_QUIT ||
				player.get() != nullptr &&
				player->gameState == gameStateQuit)//if the window was told to close or player used the quit option
			{
				quit = true;
				break;
			}
		}
	}

	close();
	return false;
}

void Game::update()
{
	readInput();

	//if ingame
	if (player->nextFloor == false)
		map.run(&resourceManager, player, memoryTextArray, gameView);

	else//if not, reset map, create a new one and place the player
	{
		map.nextFloor(player, memoryTextArray);
		map.create(player, gameView);
		player->setGameViewAndDrawXY(gameView);
	}

	menuState();
	createTopMenu();
}

void Game::readInput()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	int moveTimerSpeed = 120;

	if (keystate[SDL_SCANCODE_KP_7])
	{
		if (upLeftKey == false)
			upLeftKey = true;

		else if (upLeftKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			upLeftKey = true;
		}

		else if (upLeftKey == true)
		{
			moveTimer = 0;
			upLeftKey = keyHeld;
		}
	}

	else
		upLeftKey = false;

	if (keystate[SDL_SCANCODE_KP_9])
	{
		if (upRightKey == false)
			upRightKey = true;

		else if (upRightKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			upRightKey = true;
		}

		else if (upRightKey == true)
		{
			moveTimer = 0;
			upRightKey = keyHeld;
		}
	}

	else
		upRightKey = false;
	
	if (keystate[SDL_SCANCODE_KP_1])
	{
		if (downLeftKey == false)
			downLeftKey = true;

		else if (downLeftKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			downLeftKey = true;
		}

		else if (downLeftKey == true)
		{
			moveTimer = 0;
			downLeftKey = keyHeld;
		}
	}

	else
		downLeftKey = false;

	if (keystate[SDL_SCANCODE_KP_3])
	{
		if (downRightKey == false)
			downRightKey = true;

		else if (downRightKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			downRightKey = true;
		}

		else if (downRightKey == true)
		{
			moveTimer = 0;
			downRightKey = keyHeld;
		}
	}

	else
		downRightKey = false;

	if (keystate[SDL_SCANCODE_UP] ||
		keystate[SDL_SCANCODE_KP_8])
	{
		if (upKey == false)
			upKey = true;

		else if (upKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			upKey = true;
		}

		else if (upKey == true)
		{
			moveTimer = 0;
			upKey = keyHeld;
		}
	}

	else
		upKey = false;

	if (keystate[SDL_SCANCODE_DOWN] ||
		keystate[SDL_SCANCODE_KP_2])
	{
		if (downKey == false)
			downKey = true;

		else if (downKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			downKey = true;
		}

		else if (downKey == true)
		{
			moveTimer = 0;
			downKey = keyHeld;
		}
	}

	else
		downKey = false;

	if (keystate[SDL_SCANCODE_LEFT] ||
		keystate[SDL_SCANCODE_KP_4])
	{
		if (leftKey == false)
			leftKey = true;

		else if (leftKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			leftKey = true;
		}

		else if (leftKey == true)
		{
			moveTimer = 0;
			leftKey = keyHeld;
		}
	}

	else
		leftKey = false;

	if (keystate[SDL_SCANCODE_RIGHT] ||
		keystate[SDL_SCANCODE_KP_6])
	{
		if (rightKey == false)
			rightKey = true;

		else if (rightKey == keyHeld &&
			moveTimer >= moveTimerSpeed)
		{
			moveTimer = 0;
			rightKey = true;
		}

		else if (rightKey == true)
		{
			moveTimer = 0;
			rightKey = keyHeld;
		}
	}

	else
		rightKey = false;

	if (keystate[SDL_SCANCODE_Z])
	{
		if (attackKey == false)
			attackKey = true;

		else if (attackKey == true)
			attackKey = keyHeld;
	}

	else
		attackKey = false;

	if (keystate[SDL_SCANCODE_X])
	{
		if (facingKey == false)
			facingKey = true;

		else if (facingKey == true)
			facingKey = keyHeld;
	}

	else
		facingKey = false;

	if (keystate[SDL_SCANCODE_C])
	{
		if (menuKey == false)
			menuKey = true;

		else if (menuKey == true)
			menuKey = keyHeld;
	}

	else
		menuKey = false;

	if (keystate[SDL_SCANCODE_V])
	{
		if (diagonalKey == false)
			diagonalKey = true;

		else if (diagonalKey == true)
			diagonalKey = keyHeld;
	}

	else
		diagonalKey = false;

	if (player.get() != nullptr)
	{
		player->upLeftKey = upLeftKey;
		player->upRightKey = upRightKey;
		player->downLeftKey = downLeftKey;
		player->downRightKey = downRightKey;
		player->upKey = upKey;
		player->downKey = downKey;
		player->leftKey = leftKey;
		player->rightKey = rightKey;
		player->attackKey = attackKey;
		player->facingKey = facingKey;
		player->menuKey = menuKey;
		player->diagonalKey = diagonalKey;
	}
}

void Game::menuState()
{
	createMemoryMenu();

	if (player.get() != nullptr)
	{
		if (player->gameState == gameStateIngame)
		{
			if (playerMenuCreated == true ||
				inventoryMenuCreated == true ||
				itemMenuCreated == true ||
				floorMenuCreated == true ||
				shopMenuCreated == true)
			{
				setMenusFalse();
			}
		}

		else if (player->gameState != gameStateIngame)
		{
			if (player->gameState == gameStatePlayerMenu)
			{
				if (playerMenuCreated == false)
				{
					setMenusFalse();
					playerMenuCreated = true;
					createPlayerMenu();
					createEquipMenu();
				}

				else if (player->upKey == true ||
					player->downKey == true ||
					player->attackKey == true)
				{
					createPlayerMenu();
				}
			}

			else if (player->gameState == gameStateInventory ||
				player->gameState == gameStateShipping ||
				player->gameState == gameStateIdentifyItem ||
				player->gameState == gameStateDestroyItem)
			{
				if (inventoryMenuCreated == false ||
					memoryOfInventorySize != player->inventory.size() ||
					itemMenuCreated == true)
				{
					setMenusFalse();
					inventoryMenuCreated = true;
					memoryOfInventorySize = player->inventory.size();
					createInventoryMenu();
					createEquipMenu();
				}

				else if (player->upKey == true ||
					player->downKey == true ||
					player->attackKey == true)
				{
					createInventoryMenu();
				}
			}

			else if (player->gameState == gameStateItemMenu)
			{
				if (itemMenuCreated == false)
				{
					setMenusFalse();
					inventoryMenuCreated = true;
					itemMenuCreated = true;
					createItemMenu();
				}

				else if (player->upKey == true ||
					player->downKey == true ||
					player->attackKey == true)
				{
					createItemMenu();
				}
			}

			else if (player->gameState == gameStateFloorMenu)
			{
				if (floorMenuCreated == false)
				{
					setMenusFalse();
					floorMenuCreated = true;
					createFloorMenu();
					createEquipMenu();
				}

				else if (player->upKey == true ||
					player->downKey == true ||
					player->attackKey == true)
				{
					createFloorMenu();
				}
			}

			else if (player->gameState == gameStateShopMenu)
			{
				if (shopMenuCreated == false)
				{
					setMenusFalse();
					shopMenuCreated = true;
					createShopMenu();
					createEquipMenu();
				}

				else if (player->upKey == true ||
					player->downKey == true ||
					player->attackKey == true)
				{
					createShopMenu();
				}
			}

			else if (player->gameState == gameStateMainMenu)
			{
				//not made yet
			}
		}
	}
}

void Game::createMemoryMenu()
{
	if (memoryOfMemoryMenuSize != memoryTextArray.size())
	{
		memoryMenuTimer = 0;

		memoryMenu.create(1, gameView.h - 30, gameView.w - 2, 29, true);

		if (memoryTextArray.size() > 4)//clear anything in memoryText beyond the 4th entry
			memoryTextArray.erase(memoryTextArray.begin() + 4, memoryTextArray.end());
		memoryOfMemoryMenuSize = memoryTextArray.size();

		for (int i = 0; i < memoryTextArray.size(); ++i)
		{
			memoryMenu.addText(memoryTextArray[i]->c_str(), memoryMenu.textboxPtr->textboxRect.x + 2, memoryMenu.textboxPtr->textboxRect.y + 24 - (i * 8));
		}
	}
}

void Game::setMenusFalse()
{
	playerMenuCreated = false;
	inventoryMenuCreated = false;
	itemMenuCreated = false;
	floorMenuCreated = false;
	shopMenuCreated = false;
}

void Game::createPlayerMenu()
{
	int heightIncrement = 0;
	int menuHeight = 18;
	
	if (player->equippedItem.get() != nullptr &&
		player->equippedItem->slottedCrop.get() != nullptr &&
		player->equippedItem->strength > 0)
		menuHeight += 6;

	if (player->standingOnObject == true)
		menuHeight += 6;

	int menuY = 8 + round(28.5 - (double)menuHeight / 2);

	playerMenu.create(1, menuY, 48, menuHeight, true);

	if (player->menuState != playerMenuInventory)
		playerMenu.addText(" Inventory", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);
	else
		playerMenu.addText(">Inventory", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);

	heightIncrement += 6;

	if (player->equippedItem.get() != nullptr &&
		player->equippedItem->slottedCrop.get() != nullptr &&
		player->equippedItem->strength > 0)
	{
		if (player->menuState != playerMenuUse)
			playerMenu.addText(" Use Skill", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);
		else
			playerMenu.addText(">Use Skill", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);

		heightIncrement += 6;
	}

	if (player->standingOnObject == true)
	{
		if (player->menuState != playerMenuFloor)
			playerMenu.addText(" Floor", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);
		else
			playerMenu.addText(">Floor", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);

		heightIncrement += 6;
	}

	if (player->menuState != playerMenuQuit)
		playerMenu.addText(" Quit", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);
	else
		playerMenu.addText(">Quit", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);

	heightIncrement += 6;

	playerMenu.addText("[  " + std::to_string(player->hungerSatiety) + "/100]", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + heightIncrement);
	playerMenu.iconArray.push_back(std::make_unique<BaseThing>(playerMenu.textboxPtr->textboxRect.x + 5, playerMenu.textboxPtr->textboxRect.y + heightIncrement - 3, 104, 40));
}

void Game::createInventoryMenu()
{
	playerMenu.create(1, 8, gameView.w - 2, 59, true);
		
	for (int i = 0; i < player->inventory.size(); ++i)
	{
		bool selected = false;

		if (player->menuState == i)
		{
			selected = true;
			playerMenu.addText(">", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + 2 + (i * 12));
		}

		std::string strengthLevel = "";

		if(player->inventory[i]->itemType == cropItemType)
		{
			if (player->inventory[i]->strength == 0)
				strengthLevel = "C";
			else if (player->inventory[i]->strength == 1)
				strengthLevel = "B";
			else if (player->inventory[i]->strength == 2)
				strengthLevel = "A";
			else
				strengthLevel = "S";

			strengthLevel = "Grade " + strengthLevel + " ";
		}

		playerMenu.iconArray.push_back(std::make_unique<BaseThing>(playerMenu.textboxPtr->textboxRect.x + 6, playerMenu.textboxPtr->textboxRect.y + 2 + (i * 12), player->inventory[i]->spriteSheetRect.x, player->inventory[i]->spriteSheetRect.y));
		
		if (selected == false)
			playerMenu.addText(strengthLevel + player->inventory[i]->name + player->inventory[i]->addName, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + 4 + (i * 12));
		else if (player->gameState == gameStateShipping &&
			player->inventory[i]->itemType == cropItemType)
		{
			playerMenu.addText(strengthLevel + player->inventory[i]->name + player->inventory[i]->addName, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + (i * 12));
			playerMenu.addText(std::to_string(player->inventory[i]->price) + "G/" + std::to_string(player->money) + "G", playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + 6 + (i * 12));
		}

		else
		{
			playerMenu.addText(strengthLevel + player->inventory[i]->name + player->inventory[i]->addName, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + (i * 12));
			playerMenu.addText(player->inventory[i]->description, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + 6 + (i * 12));
		}
	}
}

void Game::createItemMenu()
{
	itemMenu.create(playerMenu.textboxPtr->textboxRect.x + 12, playerMenu.textboxPtr->textboxRect.y + 6 + (player->menuState * 12), 28, 17, true);

	if (player->itemMenuState != itemMenuUse)
		itemMenu.addText(" Use", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y);
	else
		itemMenu.addText(">Use", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y);

	if (player->itemMenuState != itemMenuThrow)
		itemMenu.addText(" Throw", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y + 6);
	else
		itemMenu.addText(">Throw", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y + 6);

	if (player->itemMenuState != itemMenuDrop)
		itemMenu.addText(" Drop", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y + 12);
	else
		itemMenu.addText(">Drop", itemMenu.textboxPtr->textboxRect.x + 2, itemMenu.textboxPtr->textboxRect.y + 12);
}

void Game::createFloorMenu()
{
	int menuHeight = 11;
	int menuY = 8 + round(28.5 - (double)menuHeight / 2);

	playerMenu.create(1, menuY, 32, menuHeight, true);

	if (player->menuState != floorMenuEquip)
		playerMenu.addText(" Equip", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y);
	else
		playerMenu.addText(">Equip", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y);

	if (player->menuState != floorMenuIgnore)
		playerMenu.addText(" Ignore", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + 6);
	else
		playerMenu.addText(">Ignore", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + 6);
}

void Game::createShopMenu()
{
	playerMenu.create(1, 8, gameView.w - 2, 59, true);

	for (int i = 0; i < map.shopInventory.size(); ++i)
	{
		bool selected = false;

		if (map.shopInventory[i]->itemType == seedItemType)
		{
			bool identified = false;

			for (int p = 0; p < player->identifiedPlants.size(); ++p)
			{
				if (map.shopInventory[i]->subType == player->identifiedPlants[p])
				{
					identified = true;
					map.shopInventory[i]->identifyPlant(true);
					map.shopInventory[i]->price = 20;
					break;
				}
			}

			if (identified == false)
			{
				map.shopInventory[i]->identifyPlant(false);
				map.shopInventory[i]->price = 10;
			}
		}

		if (player->menuState == i)
		{
			selected = true;
			playerMenu.addText(">", playerMenu.textboxPtr->textboxRect.x + 2, playerMenu.textboxPtr->textboxRect.y + 2 + (i * 12));
		}

		playerMenu.iconArray.push_back(std::make_unique<BaseThing>(playerMenu.textboxPtr->textboxRect.x + 6, playerMenu.textboxPtr->textboxRect.y + 2 + (i * 12), map.shopInventory[i]->spriteSheetRect.x, map.shopInventory[i]->spriteSheetRect.y));

		if (selected == false)
			playerMenu.addText(map.shopInventory[i]->name + map.shopInventory[i]->addName, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + 4 + (i * 12));
		else
		{
			playerMenu.addText(map.shopInventory[i]->name + map.shopInventory[i]->addName, playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + (i * 12));
			playerMenu.addText(std::to_string(map.shopInventory[i]->price) + "G/" + std::to_string(player->money) + "G", playerMenu.textboxPtr->textboxRect.x + 15, playerMenu.textboxPtr->textboxRect.y + 6 + (i * 12));
		}
	}
}

void Game::createTopMenu()
{
	if (memoryOfMapSeason != map.season ||
		memoryOfPlayerHealth != player->health ||
		memoryOfPlayerMaxHealth != player->maxHealth ||
		memoryOfPlayerMoney != player->money)
	{
		memoryOfMapSeason = map.season;
		memoryOfPlayerHealth = player->health;
		memoryOfPlayerMaxHealth = player->maxHealth;
		memoryOfPlayerMoney = player->money;

		int healthNumber = player->health;
		
		if (player->health <= 0)
			healthNumber = 0;

		std::string moneyText = std::to_string(player->money) + "G  ";
		int moneyTextLocation = gameView.w - (moneyText.length() * 4) - 4;

		topMenu.create(0, 0, gameView.w, 8, false);
		topMenu.iconArray.push_back(std::make_unique<BaseThing>(4, -1, map.seasonIcon.spriteSheetRect.x, map.seasonIcon.spriteSheetRect.y));
		topMenu.addText("  YEAR " + std::to_string(map.year), 4, 2);		
		topMenu.iconArray.push_back(std::make_unique<BaseThing>(gameView.w - 12, -1, 112, 40));
		topMenu.addText(moneyText, moneyTextLocation, 2);
		topMenu.addText(std::to_string(healthNumber) + "/" + std::to_string(player->maxHealth) + "HP", 56, 2);
	}
}

void Game::createEquipMenu()
{
	if (player.get() != nullptr &&
		player->equippedItem.get() != nullptr)
	{
		equipMenu.create(1, gameView.w - 60, gameView.w - 2, 29, true);

		int equipAtk = 0;
		int equipDef = 0;

		equipMenu.addText("[Pwr] " + std::to_string(player->equippedItem->strength) + "/4", equipMenu.textboxPtr->textboxRect.x + 56, equipMenu.textboxPtr->textboxRect.y + 6);

		if (player->equippedItem->skill != noStatEquipSkill)
		{
			equipAtk = floor((double)player->equippedItem->strength / 2);
			equipDef = floor((double)player->equippedItem->strength / 4);
		}

		equipMenu.iconArray.push_back(std::make_unique<BaseThing>(equipMenu.textboxPtr->textboxRect.x + 2, equipMenu.textboxPtr->textboxRect.y + 6, player->equippedItem->spriteSheetRect.x, player->equippedItem->spriteSheetRect.y));
		equipMenu.addText("[Atk] " + std::to_string((int)(player->tempAttackPower + equipAtk)), equipMenu.textboxPtr->textboxRect.x + 12, equipMenu.textboxPtr->textboxRect.y + 6);
		equipMenu.addText("[Def] " + std::to_string((int)(player->tempDefensePower + equipDef)), equipMenu.textboxPtr->textboxRect.x + 12, equipMenu.textboxPtr->textboxRect.y + 12);
		equipMenu.addText("[Skl] " + player->equippedItem->skillDescription, equipMenu.textboxPtr->textboxRect.x + 12, equipMenu.textboxPtr->textboxRect.y + 18);
		
		if (player->equippedItem.get() != nullptr &&
			player->equippedItem->slottedCrop.get() != nullptr)
		{
			equipMenu.addText(player->equippedItem->slottedCrop->name, equipMenu.textboxPtr->textboxRect.x + 2, equipMenu.textboxPtr->textboxRect.y);
			equipMenu.iconArray.push_back(std::make_unique<BaseThing>(equipMenu.textboxPtr->textboxRect.x + 2, equipMenu.textboxPtr->textboxRect.y + 15, player->equippedItem->slottedCrop->spriteSheetRect.x, player->equippedItem->slottedCrop->spriteSheetRect.y));
			equipMenu.addText("[Use] " + player->equippedItem->slottedCrop->activeAbilityDescription, equipMenu.textboxPtr->textboxRect.x + 12, equipMenu.textboxPtr->textboxRect.y + 24);
		}
	}
}

void Game::draw()
{
	//draw based on gamestate

	if (player.get() != nullptr &&
		map.exists == true)//if ingame
	{
		if (player->nextFloor == false)//hides everything but menus when you go to sleep and a new level is generating
		{
			drawLayers();

			player->draw(gRenderer, &resourceManager.txSpriteSheet, gameView);

			for (int i = 0; i < map.roomArray.size(); ++i)
			{
				for (int j = 0; j < map.roomArray[i]->growingPlantArray.size(); ++j)
				{
					if (map.roomArray[i]->growingPlantArray[j]->exists == true)
						map.roomArray[i]->growingPlantArray[j]->drawEffect(player->status.stopTime, gRenderer, &resourceManager.txSpriteSheet);
				}

				for (int j = 0; j < map.roomArray[i]->actorArray.size(); ++j)
				{
					if (map.roomArray[i]->actorArray[j]->status.exists == true)
						map.roomArray[i]->actorArray[j]->drawStatusEffects(player->status.stopTime, gRenderer, &resourceManager.txSpriteSheet);
				}
			}

			if (player->status.exists == true)
				player->drawStatusEffects(false, gRenderer, &resourceManager.txSpriteSheet);

			if (map.weatherEffect == rainingWeather &&
				map.roomArray[0]->isInRoomAndDoorway(player->x, player->y) == false ||
				map.weatherEffect == stormingWeather)
			{
				for (int i = 0; i < map.rainArray.size(); ++i)
				{
					map.rainArray[i]->drawRealPosition(gRenderer, &resourceManager.txSpriteSheet);
				}
			}

			topMenu.draw(gRenderer, &resourceManager.txGameText, &resourceManager.txSpriteSheet);
		}

		if (playerMenuCreated == true ||
			inventoryMenuCreated == true ||
			floorMenuCreated == true ||
			shopMenuCreated == true)
		{
			equipMenu.draw(gRenderer, &resourceManager.txGameText, &resourceManager.txSpriteSheet);
			playerMenu.draw(gRenderer, &resourceManager.txGameText, &resourceManager.txSpriteSheet);
		}

		if (memoryMenuTimer < 50 ||
			playerMenuCreated == true ||
			inventoryMenuCreated == true ||
			floorMenuCreated == true ||
			shopMenuCreated == true ||
			player.get() != nullptr &&
			player->status.exists == false)//50 frames
		{
			memoryMenuTimer++;
			memoryMenu.draw(gRenderer, &resourceManager.txGameText, &resourceManager.txSpriteSheet);
		}

		if (itemMenuCreated == true)
			itemMenu.draw(gRenderer, &resourceManager.txGameText, &resourceManager.txSpriteSheet);
	}
}

void Game::drawLayers()
{
	for (int i = 0; i < map.roomArray.size(); ++i)
	{
		map.roomArray[i]->drawTiles(gRenderer, &resourceManager.txSpriteSheet, gameView);
	}

	for (int i = 0; i < map.roomArray.size(); ++i)
	{
		for (int j = 0; j < map.roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (map.roomArray[i]->growingPlantArray[j]->exists == true)
				map.roomArray[i]->growingPlantArray[j]->drawMapPosition(gRenderer, &resourceManager.txSpriteSheet, gameView);
		}
	}

	for (int i = 0; i < map.roomArray.size(); ++i)
	{
		for (int j = 0; j < map.roomArray[i]->itemArray.size(); ++j)
		{
			map.roomArray[i]->itemArray[j]->drawMapPosition(gRenderer, &resourceManager.txSpriteSheet, gameView);
		}
	}

	for (int i = 0; i < map.roomArray.size(); ++i)
	{
		for (int j = 0; j < map.roomArray[i]->actorArray.size(); ++j)
		{
			if (map.roomArray[i]->actorArray[j]->status.exists == true)
				map.roomArray[i]->actorArray[j]->draw(gRenderer, &resourceManager.txSpriteSheet, gameView);
		}
	}
}

void Game::close()
{
	player.reset();//delete unique pointers

	topMenu.free();//clear menus
	playerMenu.free();
	itemMenu.free();
	equipMenu.free();
	memoryMenu.free();

	memoryTextArray.erase(memoryTextArray.begin(), memoryTextArray.end());//clear arrays

	resourceManager.free();//free loaded images
	map.free();//clear roomArray

	SDL_DestroyRenderer(gRenderer);//destroy window
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	IMG_Quit();
	SDL_Quit();//quit SDL subsystems
}