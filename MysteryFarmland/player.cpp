#include "player.h"
#include "room.h"

Player::Player()
{
	create();

	for (int i = 0; i < 3; ++i)
	{
		inventory.push_back(std::make_unique<Item>(seedItemType, getRandomInt(0, maxPlantTypeListSize - 1)));
	}
}

Player::Player(int x, int y)
{
	place(x, y);
}

Player::~Player()
{
	free();
}

void Player::free()
{
	equippedItem.reset();
	inventory.erase(inventory.begin(), inventory.end());
}

void Player::create()
{
	spriteSheetRect.x = 8;
	spriteSheetRect.y = 16;
	facingArrowSpriteSheetRect.w = 8;
	facingArrowSpriteSheetRect.h = 8;
	visible = true;

	nextFloor = false;
	playerTurn = true;
	gameState = gameStateIngame;
	menuState = 0;
	itemMenuState = 0;
	justMoved = false;
	preciseStep = false;
	attackAnimating = false;

	baseMaxHealth = 10;
	maxHealth = baseMaxHealth;
	health = maxHealth;
	baseAttackPower = 3;
	baseDefensePower = 0;
	healthRegenerationMultiplier = 1;
	satietyDegenMultiplier = 1;
	hungerSatiety = 100;
	maxExp = 22;
	money = 0;
	trueMansWorld = true;

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

void Player::place(int& x, int& y)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();
}

void Player::setNextFloor(std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	status.removeAllStatusEffects();

	tempAttackPower = 0;
	tempDefensePower = 0;
}

void Player::setFacing(int facingX, int facingY, bool animateAttack)
{
	if (x == facingX &&
		y > facingY)
	{
		facing = facingUp;

		if (animateAttack == true)
			animateAttackMove(0, -1);
	}

	else if (x > facingX &&
		y > facingY)
	{
		facing = facingUpLeft;

		if (animateAttack == true)
			animateAttackMove(-1, -1);
	}

	else if (x > facingX &&
		y == facingY)
	{
		facing = facingLeft;

		if (animateAttack == true)
			animateAttackMove(-1, 0);
	}

	else if (x > facingX &&
		y < facingY)
	{
		facing = facingDownLeft;

		if (animateAttack == true)
			animateAttackMove(-1, 1);
	}

	else if (x == facingX &&
		y < facingY)
	{
		facing = facingDown;

		if (animateAttack == true)
			animateAttackMove(0, 1);
	}

	else if (x < facingX &&
		y < facingY)
	{
		facing = facingDownRight;

		if (animateAttack == true)
			animateAttackMove(1, 1);
	}

	else if (x < facingX &&
		y == facingY)
	{
		facing = facingRight;

		if (animateAttack == true)
			animateAttackMove(1, 0);
	}

	else if (x < facingX &&
		y > facingY)
	{
		facing = facingUpRight;

		if (animateAttack == true)
			animateAttackMove(1, -1);
	}
}

void Player::targetFacing(int& targetX, int& targetY, bool animateAttack)
{
	if (facing == facingUpLeft)
	{
		targetX = -1;
		targetY = -1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingUpRight)
	{
		targetX = 1;
		targetY = -1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingDownLeft)
	{
		targetX = -1;
		targetY = 1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingDownRight)
	{
		targetX = 1;
		targetY = 1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingUp)
	{
		targetX = 0;
		targetY = -1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingLeft)
	{
		targetX = -1;
		targetY = 0;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingRight)
	{
		targetX = 1;
		targetY = 0;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}

	else if (facing == facingDown)
	{
		targetX = 0;
		targetY = 1;

		if (animateAttack == true)
		{
			attackAnimating = true;
			animateAttackMove(targetX, targetY);
		}
	}
}

void Player::preciseMovementInput(std::vector<std::unique_ptr<Room>>& roomArray)
{
	int targetX = 0;
	int targetY = 0;

	if (facingKey != keyHeld ||
		preciseStep != true)
	{
		if (status.confused >= true)
			drunkWalk();

		if (upLeftKey == true)
		{
			facing = facingUpLeft;

			targetFacing(targetX, targetY, false);

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->x == x + targetX &&
						roomArray[i]->actorArray[j]->y == y + targetY &&
						checkDiagonalSides(x - 1, y, x, y - 1, roomArray) == true)
					{
						int swapX = roomArray[i]->actorArray[j]->x;
						int swapY = roomArray[i]->actorArray[j]->y;
						int actorTargetX = 0;
						int actorTargetY = 0;
						roomArray[i]->actorArray[j]->setFacing(x, y, false);
						roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
						roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
						roomArray[i]->actorArray[j]->x = x;
						roomArray[i]->actorArray[j]->y = y;
						roomArray[i]->actorArray[j]->status.asleep = false;
						setLastXYOnMove(targetX, targetY);
						x = swapX;
						y = swapY;
						onMove(true);
						endTurn();
						return;
					}
				}
			}

			if (isTileFreeDiagonal(x - 1, y - 1, x - 1, y, x, y - 1, roomArray, true) == true)
			{
				setLastXYOnMove(-1, -1);
				x--;
				y--;
				onMove(true);
				endTurn();
				return;
			}
		}

		else if (upRightKey == true)
		{
			facing = facingUpRight;

			targetFacing(targetX, targetY, false);

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->x == x + targetX &&
						roomArray[i]->actorArray[j]->y == y + targetY &&
						checkDiagonalSides(x, y - 1, x + 1, y, roomArray) == true)
					{
						int swapX = roomArray[i]->actorArray[j]->x;
						int swapY = roomArray[i]->actorArray[j]->y;
						int actorTargetX = 0;
						int actorTargetY = 0;
						roomArray[i]->actorArray[j]->setFacing(x, y, false);
						roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
						roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
						roomArray[i]->actorArray[j]->x = x;
						roomArray[i]->actorArray[j]->y = y;
						roomArray[i]->actorArray[j]->status.asleep = false;
						setLastXYOnMove(targetX, targetY);
						x = swapX;
						y = swapY;
						onMove(true);
						endTurn();
						return;
					}
				}
			}

			if (isTileFreeDiagonal(x + 1, y - 1, x, y - 1, x + 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(1, -1);
				x++;
				y--;
				onMove(true);
				endTurn();
				return;
			}
		}

		else if (downLeftKey == true)
		{
			facing = facingDownLeft;

			targetFacing(targetX, targetY, false);

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->x == x + targetX &&
						roomArray[i]->actorArray[j]->y == y + targetY &&
						checkDiagonalSides(x, y + 1, x - 1, y, roomArray) == true)
					{
						int swapX = roomArray[i]->actorArray[j]->x;
						int swapY = roomArray[i]->actorArray[j]->y;
						int actorTargetX = 0;
						int actorTargetY = 0;
						roomArray[i]->actorArray[j]->setFacing(x, y, false);
						roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
						roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
						roomArray[i]->actorArray[j]->x = x;
						roomArray[i]->actorArray[j]->y = y;
						roomArray[i]->actorArray[j]->status.asleep = false;
						setLastXYOnMove(targetX, targetY);
						x = swapX;
						y = swapY;
						onMove(true);
						endTurn();
						return;
					}
				}
			}

			if (isTileFreeDiagonal(x - 1, y + 1, x, y + 1, x - 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(-1, 1);
				x--;
				y++;
				onMove(true);
				endTurn();
				return;
			}
		}

		else if (downRightKey == true)
		{
			facing = facingDownRight;

			targetFacing(targetX, targetY, false);

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->x == x + targetX &&
						roomArray[i]->actorArray[j]->y == y + targetY &&
						checkDiagonalSides(x, y + 1, x + 1, y, roomArray) == true)
					{
						int swapX = roomArray[i]->actorArray[j]->x;
						int swapY = roomArray[i]->actorArray[j]->y;
						int actorTargetX = 0;
						int actorTargetY = 0;
						roomArray[i]->actorArray[j]->setFacing(x, y, false);
						roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
						roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
						roomArray[i]->actorArray[j]->x = x;
						roomArray[i]->actorArray[j]->y = y;
						roomArray[i]->actorArray[j]->status.asleep = false;
						setLastXYOnMove(targetX, targetY);
						x = swapX;
						y = swapY;
						onMove(true);
						endTurn();
						return;
					}
				}
			}

			if (isTileFreeDiagonal(x + 1, y + 1, x, y + 1, x + 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(1, 1);
				x++;
				y++;
				onMove(true);
				endTurn();
				return;
			}
		}

		else if (diagonalKey == false ||
			status.confused >= true)
		{
			if (upKey == true)
			{
				facing = facingUp;

				targetFacing(targetX, targetY, false);

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->x == x + targetX &&
							roomArray[i]->actorArray[j]->y == y + targetY)
						{
							int swapX = roomArray[i]->actorArray[j]->x;
							int swapY = roomArray[i]->actorArray[j]->y;
							int actorTargetX = 0;
							int actorTargetY = 0;
							roomArray[i]->actorArray[j]->setFacing(x, y, false);
							roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
							roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
							roomArray[i]->actorArray[j]->x = x;
							roomArray[i]->actorArray[j]->y = y;
							roomArray[i]->actorArray[j]->status.asleep = false;
							setLastXYOnMove(targetX, targetY);
							x = swapX;
							y = swapY;
							onMove(true);
							endTurn();
							return;
						}
					}
				}

				if (isTileFree(x, y - 1, roomArray) == true)
				{
					setLastXYOnMove(0, -1);
					y--;
					onMove(true);
					endTurn();
					return;
				}
			}

			else if (leftKey == true)
			{
				facing = facingLeft;

				targetFacing(targetX, targetY, false);

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->x == x + targetX &&
							roomArray[i]->actorArray[j]->y == y + targetY)
						{
							int swapX = roomArray[i]->actorArray[j]->x;
							int swapY = roomArray[i]->actorArray[j]->y;
							int actorTargetX = 0;
							int actorTargetY = 0;
							roomArray[i]->actorArray[j]->setFacing(x, y, false);
							roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
							roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
							roomArray[i]->actorArray[j]->x = x;
							roomArray[i]->actorArray[j]->y = y;
							roomArray[i]->actorArray[j]->status.asleep = false;
							setLastXYOnMove(targetX, targetY);
							x = swapX;
							y = swapY;
							onMove(true);
							endTurn();
							return;
						}
					}
				}

				if (isTileFree(x - 1, y, roomArray) == true)
				{
					setLastXYOnMove(-1, 0);
					x--;
					onMove(true);
					endTurn();
					return;
				}
			}

			else if (downKey == true)
			{
				facing = facingDown;

				targetFacing(targetX, targetY, false);

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->x == x + targetX &&
							roomArray[i]->actorArray[j]->y == y + targetY)
						{
							int swapX = roomArray[i]->actorArray[j]->x;
							int swapY = roomArray[i]->actorArray[j]->y;
							int actorTargetX = 0;
							int actorTargetY = 0;
							roomArray[i]->actorArray[j]->setFacing(x, y, false);
							roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
							roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
							roomArray[i]->actorArray[j]->x = x;
							roomArray[i]->actorArray[j]->y = y;
							roomArray[i]->actorArray[j]->status.asleep = false;
							setLastXYOnMove(targetX, targetY);
							x = swapX;
							y = swapY;
							onMove(true);
							endTurn();
							return;
						}
					}
				}

				if (isTileFree(x, y + 1, roomArray) == true)
				{
					setLastXYOnMove(0, 1);
					y++;
					onMove(true);
					endTurn();
					return;
				}
			}

			else if (rightKey == true)
			{
				facing = facingRight;

				targetFacing(targetX, targetY, false);

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->x == x + targetX &&
							roomArray[i]->actorArray[j]->y == y + targetY)
						{
							int swapX = roomArray[i]->actorArray[j]->x;
							int swapY = roomArray[i]->actorArray[j]->y;
							int actorTargetX = 0;
							int actorTargetY = 0;
							roomArray[i]->actorArray[j]->setFacing(x, y, false);
							roomArray[i]->actorArray[j]->targetFacing(actorTargetX, actorTargetY, false);
							roomArray[i]->actorArray[j]->setLastXYOnMove(actorTargetX, actorTargetY);
							roomArray[i]->actorArray[j]->x = x;
							roomArray[i]->actorArray[j]->y = y;
							roomArray[i]->actorArray[j]->status.asleep = false;
							setLastXYOnMove(targetX, targetY);
							x = swapX;
							y = swapY;
							onMove(true);
							endTurn();
							return;
						}
					}
				}

				if (isTileFree(x + 1, y, roomArray) == true)
				{
					setLastXYOnMove(1, 0);
					x++;
					onMove(true);
					endTurn();
					return;
				}
			}
		}
	}

	//else just movementInput with isPreciseStepping true
}

void Player::movementInput(bool isPreciseStepping, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (facingKey != keyHeld ||
		preciseStep != true)
	{
		if (status.confused >= true)
			drunkWalk();

		if (upLeftKey == true)
		{
			facing = facingUpLeft;

			if (isTileFreeDiagonal(x - 1, y - 1, x - 1, y, x, y - 1, roomArray, true) == true)
			{
				setLastXYOnMove(-1, -1);
				x--;
				y--;
				onMove(isPreciseStepping);
				endTurn();
				return;
			}
		}

		else if (upRightKey == true)
		{
			facing = facingUpRight;

			if (isTileFreeDiagonal(x + 1, y - 1, x, y - 1, x + 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(1, -1);
				x++;
				y--;
				onMove(isPreciseStepping);
				endTurn();
				return;
			}
		}

		else if (downLeftKey == true)
		{
			facing = facingDownLeft;

			if (isTileFreeDiagonal(x - 1, y + 1, x, y + 1, x - 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(-1, 1);
				x--;
				y++;
				onMove(isPreciseStepping);
				endTurn();
				return;
			}
		}

		else if (downRightKey == true)
		{
			facing = facingDownRight;

			if (isTileFreeDiagonal(x + 1, y + 1, x, y + 1, x + 1, y, roomArray, true) == true)
			{
				setLastXYOnMove(1, 1);
				x++;
				y++;
				onMove(isPreciseStepping);
				endTurn();
				return;
			}
		}

		else if (diagonalKey == false ||
			status.confused >= true)
		{
			if (upKey == true)
			{
				facing = facingUp;

				if (isTileFree(x, y - 1, roomArray) == true)
				{
					setLastXYOnMove(0, -1);
					y--;
					onMove(isPreciseStepping);
					endTurn();
					return;
				}
			}

			else if (leftKey == true)
			{
				facing = facingLeft;

				if (isTileFree(x - 1, y, roomArray) == true)
				{
					setLastXYOnMove(-1, 0);
					x--;
					onMove(isPreciseStepping);
					endTurn();
					return;
				}
			}

			else if (downKey == true)
			{
				facing = facingDown;

				if (isTileFree(x, y + 1, roomArray) == true)
				{
					setLastXYOnMove(0, 1);
					y++;
					onMove(isPreciseStepping);
					endTurn();
					return;
				}
			}

			else if (rightKey == true)
			{
				facing = facingRight;

				if (isTileFree(x + 1, y, roomArray) == true)
				{
					setLastXYOnMove(1, 0);
					x++;
					onMove(isPreciseStepping);
					endTurn();
					return;
				}
			}
		}
	}
}

void Player::onMove(bool isPreciseStepping)
{
	justMoved = true;
	standingOnObject = false;
	preciseStep = isPreciseStepping;
	attackAnimating = false;

	rumble = 0;
	drawXRelative = 0;
	drawYRelative = 0;
}

bool Player::takeDefendableDamage(int& damageTaken)
{
	if (getRandomInt(1, 100) >= dodgeChance)
	{
		if (equippedItem.get() != nullptr &&
			equippedItem->skill != noStatEquipSkill)
			damageTaken -= baseDefensePower + tempDefensePower + floor((double)equippedItem->strength / 4);
		else
			damageTaken -= baseDefensePower + tempDefensePower;

		if (status.blocking == false)
		{
			//*bump*
		}

		else if (status.blocking >= true)
		{
			//*ka-sheen*
			damageTaken = 1;
		}

		if (damageTaken < 1)
			damageTaken = 1;

		healthRegeneration = 0;
		takeTrueDamage(damageTaken);

		return true;
	}

	else
	{
		//*whoosh* dodged!
		return false;
	}
}

void Player::takeTrueDamage(int damageTaken)
{
	health -= damageTaken;
	rumble = 3;
	status.asleep = false;
	status.checkDeath(health);
}

void Player::eatFood(int nutritionGained, std::string name, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	//*chomp*
	rumble = 2;

	hungerSatiety += nutritionGained;

	if (hungerSatiety > 100)
	{
		hungerSatiety = 100;
	}

	else if (hungerSatiety < 0)
	{
		hungerSatiety = 0;
	}

	satietyDegen = 0;
	
	memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Ate " + name + ".")));

}

void Player::removeMenuStateFromInventory()
{
	inventory.erase(inventory.begin() + menuState);

	if (menuState > inventory.size() - 1)//if inventory has shrunk and menuState is bigger than it, menuState == its size (0 indexed)
		menuState = inventory.size() - 1;
}

void Player::careForPlant(bool watering, int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->alive == true)
	{
		if (watering == true)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Watered plant.")));
		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Fertilized plant.")));

		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->careForMe(watering);
	}
}

void Player::recalculatePlantIdentification(std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			bool plantIdentified = false;

			for (int p = 0; p < identifiedPlants.size(); ++p)
			{
				if (roomArray[i]->growingPlantArray[j]->grownCrop->subType == identifiedPlants[p])
					plantIdentified = true;
			}

			roomArray[i]->growingPlantArray[j]->setIdentification(plantIdentified);//if it isnt in the identified list, its unidentified
		}

		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			if (roomArray[i]->itemArray[j]->itemType == seedItemType ||
				roomArray[i]->itemArray[j]->itemType == cropItemType)
			{
				bool plantIdentified = false;

				for (int p = 0; p < identifiedPlants.size(); ++p)
				{
					if (roomArray[i]->itemArray[j]->subType == identifiedPlants[p])
						plantIdentified = true;
				}

				roomArray[i]->itemArray[j]->identifyPlant(plantIdentified);
			}
		}
	}

	for (int i = 0; i < inventory.size(); ++i)
	{
		if (inventory[i]->itemType == seedItemType ||
			inventory[i]->itemType == cropItemType)
		{
			bool plantIdentified = false;

			for (int p = 0; p < identifiedPlants.size(); ++p)
			{
				if (inventory[i]->subType == identifiedPlants[p])
					plantIdentified = true;
			}

			inventory[i]->identifyPlant(plantIdentified);
		}
	}
}

void Player::recalculateHerbIdentification(std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			if (roomArray[i]->itemArray[j]->itemType == herbItemType)
			{
				bool herbIdentified = false;

				for (int h = 0; h < identifiedHerbs.size(); ++h)
				{
					if (roomArray[i]->itemArray[j]->subType == identifiedHerbs[h])
						herbIdentified = true;
				}

				roomArray[i]->itemArray[j]->identifyHerb(herbIdentified);
			}
		}
	}

	for (int i = 0; i < inventory.size(); ++i)
	{
		if (inventory[i]->itemType == herbItemType)
		{
			bool herbIdentified = false;

			for (int h = 0; h < identifiedHerbs.size(); ++h)
			{
				if (inventory[i]->subType == identifiedHerbs[h])
					herbIdentified = true;
			}

			inventory[i]->identifyHerb(herbIdentified);
		}
	}
}

void Player::update(std::vector<std::unique_ptr<Room>>& roomArray, int& mapWidth, int& mapHeight, int& season, std::vector<std::unique_ptr<std::string>>& memoryTextArray, std::vector<std::unique_ptr<Item>>& shopInventory, std::vector<std::unique_ptr<Item>>& storedBoughtItemArray, SDL_Rect& gameView)
{
	if (statusChecked == false)//happens once per turn
		checkStatus(memoryTextArray);

	if (status.exists == true)
	{
		if (status.asleep == false &&
			status.stunned == false &&
			status.missThisTurn == false)
		{
			handleInput(roomArray, mapWidth, mapHeight, season, memoryTextArray, shopInventory, storedBoughtItemArray, gameView);
			floorInteract(roomArray, memoryTextArray);
		}

		else
			endTurn();
	}
}

void Player::checkStatus(std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	statusChecked = true;

	status.applyStatus(health, false);

	if (status.exists == true)
	{
		if (status.missThisTurn == false)
		{
			if (status.fastHunger == true)
				satietyDegenMultiplier = 2;
			else
				satietyDegenMultiplier = 1;

			if (status.asleep >= true ||
				status.fastHealthRegen == true)
				healthRegenerationMultiplier = 2;
			else
				healthRegenerationMultiplier = 1;

			if (status.asleep >= true)//wake up with lime
			{
				if (equippedItem.get() != nullptr &&
					equippedItem->slottedCrop.get() != nullptr &&
					equippedItem->slottedCrop->subType == limePlantType &&
					equippedItem->strength > 0)
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Resisted sleep.")));
					equippedItem->powerSave(memoryTextArray);
					status.asleep = false;
				}
			}

			if (status.stunned >= true)//roll for unstun
			{
				if (getRandomInt(1, 100) <= 10)
					status.stunned = false;
			}

			if (status.shrunken >= true)//shrunken hp
			{
				maxHealth = floor(baseMaxHealth / 2);

				if (health > maxHealth)
					health = maxHealth;
			}

			satietyDegen += (1 * satietyDegenMultiplier);

			if (satietyDegen >= 5)//decrease satiety
			{
				hungerSatiety--;
				satietyDegen = 0;

				if (hungerSatiety <= 0)
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I'm starving...")));
					hungerSatiety = 0;
					takeTrueDamage(1);
				}
			}

			if (health < maxHealth &&
				hungerSatiety > 0 &&
				status.poisoned == 0)//regenerate health if no health draining effects are active and health isnt maxed
			{
				healthRegeneration += (((double)maxHealth / 100) * healthRegenerationMultiplier);

				if (healthRegeneration >= 1)
				{
					health++;
					healthRegeneration = 0;
				}

				if (health > maxHealth)
					health = maxHealth;
			}

			else
				healthRegeneration = 0;
		}
	}
	
	else
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I fainted!")));
}

void Player::checkVisible(std::vector<std::unique_ptr<Room>>& roomArray, SDL_Rect& gameView)
{
	resetVisible(roomArray);

	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true ||
			status.seeAll == true)
		{
			for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
			{
				roomArray[i]->tileArray[j]->visible = true;
				roomArray[i]->tileArray[j]->discovered = true;
			}

			if (status.seeAll == true)
			{
				for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
				{
					roomArray[i]->hallwayArray[j]->visible = true;
					roomArray[i]->hallwayArray[j]->discovered = true;
				}
			}

			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (status.seeAll == true)
					roomArray[i]->roomTileArray[j]->discovered = true;

				if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap ||
					roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap &&
					roomArray[i]->roomTileArray[j]->discovered == true)
					roomArray[i]->roomTileArray[j]->visible = true;
			}

			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				roomArray[i]->growingPlantArray[j]->visible = true;
			}

			for (int r = 0; r < roomArray.size(); ++r)
			{
				for (int j = 0; j < roomArray[r]->actorArray.size(); ++j)
				{
					if (status.seeAll == true ||
						roomArray[i]->isInRoomAndDoorway(roomArray[r]->actorArray[j]->x, roomArray[r]->actorArray[j]->y) == true)
						roomArray[r]->actorArray[j]->visible = true;
				}

				for (int j = 0; j < roomArray[r]->itemArray.size(); ++j)
				{
					if (status.seeAll == true ||
						roomArray[i]->isInRoomAndDoorway(roomArray[r]->itemArray[j]->x, roomArray[r]->itemArray[j]->y) == true)
						roomArray[r]->itemArray[j]->visible = true;
				}
			}
		}

		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (isInThisDistanceOfMe(roomArray[i]->tileArray[j]->x, roomArray[i]->tileArray[j]->y, 1) == true ||
				roomArray[i]->tileArray[j]->discovered == true)
			{
				roomArray[i]->tileArray[j]->visible = true;
				roomArray[i]->tileArray[j]->discovered = true;
			}
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (isInThisDistanceOfMe(roomArray[i]->hallwayArray[j]->x, roomArray[i]->hallwayArray[j]->y, 1) == true ||
				roomArray[i]->hallwayArray[j]->discovered == true)
			{
				roomArray[i]->hallwayArray[j]->visible = true;
				roomArray[i]->hallwayArray[j]->discovered = true;
			}
		}

		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			if (isInThisDistanceOfMe(roomArray[i]->roomTileArray[j]->x, roomArray[i]->roomTileArray[j]->y, 1) == true)
			{
				if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap ||
					roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap &&
					roomArray[i]->roomTileArray[j]->discovered == true)
					roomArray[i]->roomTileArray[j]->visible = true;
				else if (roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap &&
					roomArray[i]->roomTileArray[j]->discovered == false &&
					getRandomInt(1, 100) <= 5)
					roomArray[i]->roomTileArray[j]->discovered = true;
			}
		}

		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (isInThisDistanceOfMe(roomArray[i]->growingPlantArray[j]->x, roomArray[i]->growingPlantArray[j]->y, 1) == true)
				roomArray[i]->growingPlantArray[j]->visible = true;
		}

		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (status.seeAll == true ||
				isInThisDistanceOfMe(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y, 1) == true)
				roomArray[i]->actorArray[j]->visible = true;
		}

		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			if (status.seeAll == true ||
				isInThisDistanceOfMe(roomArray[i]->itemArray[j]->x, roomArray[i]->itemArray[j]->y, 1) == true)
				roomArray[i]->itemArray[j]->visible = true;
		}
	}
}

void Player::resetVisible(std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			roomArray[i]->roomTileArray[j]->visible = false;
		}

		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			roomArray[i]->growingPlantArray[j]->visible = false;
		}

		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			roomArray[i]->actorArray[j]->visible = false;
		}

		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			roomArray[i]->itemArray[j]->visible = false;
		}
	}
}

void Player::handleInput(std::vector<std::unique_ptr<Room>>& roomArray, int& mapWidth, int& mapHeight, int& season, std::vector<std::unique_ptr<std::string>>& memoryTextArray, std::vector<std::unique_ptr<Item>>& shopInventory, std::vector<std::unique_ptr<Item>>& storedBoughtItemArray, SDL_Rect& gameView)
{
	dKeyDiagonalInput();

	if (gameState == gameStateIngame)
	{
		if (attackKey == true)
		{
			findAttackTarget(roomArray, memoryTextArray, gameView);

			if(gameState == gameStateIngame)
				endTurn();
			return;
		}

		else if (facingKey == true)
		{
			gameState = gameStateFacing;
			preciseStep = false;
		}

		else if (attackKey == keyHeld &&
			facingKey == keyHeld)
		{
			if (quickWait(roomArray) == true)
				endTurn();

			return;
		}

		else if (facingKey == keyHeld)
			preciseMovementInput(roomArray);

		else if (menuKey == true)
		{
			gameState = gameStatePlayerMenu;
			menuState = 0;
			menuKey = keyHeld;
		}

		movementInput(false, roomArray);
	}

	else if (gameState == gameStateFacing)
	{
		if (attackKey == true)
		{
			gameState = gameStateIngame;

			if (facingKey != keyHeld)
				findAttackTarget(roomArray, memoryTextArray, gameView);

			if (gameState == gameStateIngame)
				endTurn();
			return;
		}

		else if (facingKey == true)
			gameState = gameStateIngame;

		else if (facingKey == keyHeld)
			preciseMovementInput(roomArray);

		else if (menuKey == true)
		{
			gameState = gameStatePlayerMenu;
			menuState = 0;
			menuKey = keyHeld;
		}

		else
			handleFacing();
	}

	else if (gameState == gameStatePlayerMenu)
	{
		if (upKey == true)
		{
			menuState--;

			if (menuState == playerMenuFloor &&
				standingOnObject == false)
				menuState--;

			if (menuState == playerMenuUse &&
				equippedItem.get() == nullptr ||
				menuState == playerMenuUse &&
				equippedItem->slottedCrop.get() == nullptr ||
				menuState == playerMenuUse &&
				equippedItem->strength == 0)//CropActive and Floor only appear when relevant, so skip them when they arent. crop has to exist for CropActive and standingOnObject has to be true for Floor
				menuState--;

			if (menuState < 0)
				menuState = maxPlayerMenuListSize - 1;
		}

		else if (downKey == true)
		{
			menuState++;

			if (menuState == playerMenuUse &&
				equippedItem.get() == nullptr ||
				menuState == playerMenuUse &&
				equippedItem->slottedCrop.get() == nullptr ||
				menuState == playerMenuUse &&
				equippedItem->strength == 0)
				menuState++;

			if (menuState == playerMenuFloor &&
				standingOnObject == false)
				menuState++;

			if (menuState > maxPlayerMenuListSize - 1)
				menuState = 0;
		}

		if (attackKey == true)//purposely not an else if, can move and use z, x, etc. in one frame
		{
			if (menuState == playerMenuInventory)
			{
				gameState = gameStateInventory;
				menuState = 0;
				attackKey = false;
			}

			else if (menuState == playerMenuUse)
			{
				if (equippedItem.get() != nullptr &&
					equippedItem->slottedCrop.get() != nullptr &&
					equippedItem->strength > 0)
				{
					useSlottedCropAbility(roomArray, memoryTextArray, gameView);
					endTurn();
				}
			}

			else if (menuState == playerMenuFloor)
			{
				gameState = gameStateIngame;
				justMoved = true;
				preciseStep = false;
				attackKey = false;
			}

			else if (menuState == playerMenuQuit)
				gameState = gameStateQuit;
		}

		else if (facingKey == true)
			gameState = gameStateIngame;
	}

	else if (gameState == gameStateInventory ||
		gameState == gameStateShipping ||
		gameState == gameStateIdentifyItem ||
		gameState == gameStateDestroyItem)
	{
		if (upKey == true)
		{
			menuState--;

			if (menuState < 0)
				menuState = inventory.size() - 1;
		}

		else if (downKey == true)
		{
			menuState++;

			if (menuState > inventory.size() - 1)
				menuState = 0;
		}

		if (attackKey == true)
		{
			if (inventory.size() > 0)
			{
				if (gameState == gameStateInventory)
				{
					gameState = gameStateItemMenu;
					itemMenuState = itemMenuUse;
				}

				else if (gameState == gameStateShipping &&
					inventory[menuState]->itemType == cropItemType)
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Shipped " + inventory[menuState]->name + ".")));
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Made " + std::to_string(inventory[menuState]->price) + "G.")));

					int targetX;
					int targetY;
					targetFacing(targetX, targetY, true);

					money += inventory[menuState]->price;
					removeMenuStateFromInventory();
				}

				else if (gameState == gameStateIdentifyItem)
				{
					identifyMenuState(roomArray, memoryTextArray);
					gameState = gameStateIngame;
					menuState = 0;
				}

				else if (gameState == gameStateDestroyItem)
				{
					destroyMenuState(roomArray, memoryTextArray);
					gameState = gameStateIngame;
					menuState = 0;
				}
			}
		}

		else if (facingKey == true)
		{
			if (gameState == gameStateInventory)
				gameState = gameStatePlayerMenu;
			else if (gameState == gameStateIdentifyItem ||
				gameState == gameStateDestroyItem)
				endTurn();
			else
				gameState = gameStateIngame;
			menuState = 0;
		}
	}

	else if (gameState == gameStateItemMenu)
	{
		if (upKey == true)
		{
			itemMenuState--;

			if (itemMenuState < 0)
				itemMenuState = maxItemMenuListSize - 1;
		}

		else if (downKey == true)
		{
			itemMenuState++;

			if (itemMenuState > maxItemMenuListSize - 1)
				itemMenuState = itemMenuUse;
		}

		if (attackKey == true)
		{
			if (itemMenuState == itemMenuUse)
				useItem(season, roomArray, memoryTextArray, gameView);

			else if (itemMenuState == itemMenuThrow)
			{
				throwItem(roomArray, memoryTextArray);
				endTurn();
			}

			else if (itemMenuState == itemMenuDrop)
			{
				int dropX = x;
				int dropY = y;
				int returnRoom = -1;

				if (getFreeDroppingSpace(dropX, dropY, returnRoom, roomArray) == true)
				{
					inventory[menuState]->x = dropX;
					inventory[menuState]->y = dropY;
					inventory[menuState]->setDrawXY();
					roomArray[returnRoom]->itemArray.push_back(std::move(inventory[menuState]));
					removeMenuStateFromInventory();

					if (x == dropX &&
						y == dropY)
						standingOnObject = true;
					endTurn();
				}

				else
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't drop here!")));
			}
		}

		else if (facingKey == true)
		{
			gameState = gameStateInventory;
		}
	}

	else if (gameState == gameStateShopMenu)
	{
		if (upKey == true)
		{
			menuState--;

			if (menuState < 0)
				menuState = shopInventory.size() - 1;
		}

		else if (downKey == true)
		{
			menuState++;

			if (menuState > shopInventory.size() - 1)
				menuState = 0;
		}

		if (attackKey == true)
		{
			if (shopInventory.size() > 0)
			{
				if (money >= shopInventory[menuState]->price)
				{
					money -= shopInventory[menuState]->price;
					storedBoughtItemArray.push_back(std::move(shopInventory[menuState]));
					shopInventory.erase(shopInventory.begin() + menuState);

					if (menuState > shopInventory.size() - 1)
						menuState = shopInventory.size() - 1;
				}

				else
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't afford that!")));
			}
		}

		else if (facingKey == true)
		{
			gameState = gameStateIngame;
		}
	}

	else if (gameState == gameStateMainMenu)
	{
		//not made yet
	}
}

void Player::drunkWalk()
{
	if (upLeftKey == true ||
		upRightKey == true ||
		downLeftKey == true ||
		downRightKey == true ||
		upKey == true ||
		leftKey == true ||
		downKey == true ||
		rightKey == true)
	{
		upLeftKey = false;
		upRightKey = false;
		downLeftKey = false;
		downRightKey = false;
		upKey = false;
		leftKey = false;
		downKey = false;
		rightKey = false;

		int randomizedKey = getRandomInt(1, 8);

		if (randomizedKey == 1)
			upLeftKey = true;
		else if (randomizedKey == 2)
			upRightKey = true;
		else if (randomizedKey == 3)
			downLeftKey = true;
		else if (randomizedKey == 4)
			downRightKey = true;
		else if (randomizedKey == 5)
			upKey = true;
		else if (randomizedKey == 6)
			leftKey = true;
		else if (randomizedKey == 7)
			downKey = true;
		else if (randomizedKey == 8)
			rightKey = true;
	}
}

void Player::dKeyDiagonalInput()
{
	if (diagonalKey == true ||
		diagonalKey == keyHeld)
	{
		if (upKey == keyHeld)
		{
			if (leftKey == true)
				upLeftKey = true;
			else if (rightKey == true)
				upRightKey = true;
		}

		else if (downKey == keyHeld)
		{
			if (leftKey == true)
				downLeftKey = true;
			else if (rightKey == true)
				downRightKey = true;
		}

		else if (leftKey == keyHeld)
		{
			if (upKey == true)
				upLeftKey = true;
			else if (downKey == true)
				downLeftKey = true;
		}

		else if (rightKey == keyHeld)
		{
			if (upKey == true)
				upRightKey = true;
			else if (downKey == true)
				downRightKey = true;
		}
	}
}

void Player::findAttackTarget(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	if (status.fleeing == false)
	{
		int targetX = 0;
		int targetY = 0;

		if(status.confused >= true)
			facing = getRandomInt(0, maxFacingListSize - 1);
		targetFacing(targetX, targetY, true);
		attackAnimating = true;

		if (facing == facingUpLeft)
		{
			if (isTileFreeDiagonal(x - 1, y - 1, x - 1, y, x, y - 1, roomArray, false) == false)//if theres a wall to the side of the diagonal attack, attack doesnt work
				return;
		}

		else if (facing == facingDownLeft)
		{
			if (isTileFreeDiagonal(x - 1, y + 1, x - 1, y, x, y + 1, roomArray, false) == false)
				return;
		}

		else if (facing == facingDownRight)
		{
			if (isTileFreeDiagonal(x + 1, y + 1, x + 1, y, x, y + 1, roomArray, false) == false)
				return;
		}

		else if (facing == facingUpRight)
		{
			if (isTileFreeDiagonal(x + 1, y - 1, x + 1, y, x, y - 1, roomArray, false) == false)
				return;
		}

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true)
				{
					if (roomArray[i]->actorArray[j]->x == x + targetX &&
						roomArray[i]->actorArray[j]->y == y + targetY)
					{
						attackEnemy(i, j, roomArray, memoryTextArray, gameView);
						break;
					}
				}
			}

			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (roomArray[i]->roomTileArray[j]->x == x + targetX &&
					roomArray[i]->roomTileArray[j]->y == y + targetY)
				{
					attackRoomTile(i, j, roomArray, memoryTextArray);
					break;
				}
			}

			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->exists == true &&
					roomArray[i]->growingPlantArray[j]->x == x + targetX &&
					roomArray[i]->growingPlantArray[j]->y == y + targetY)
				{
					attackGrowingPlant(i, j, roomArray, memoryTextArray);
					break;
				}
			}
		}
	}

	else
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't attack while fleeing!")));
}

void Player::attackRoomTile(int& roomNumber, int& roomTileNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (roomArray[roomNumber]->roomTileArray[roomTileNumber]->tileType == tileTypeTrap)
		roomArray[roomNumber]->roomTileArray[roomTileNumber]->discovered = true;

	else if (roomArray[roomNumber]->roomTileArray[roomTileNumber]->tileType == tileTypeBed)
	{
		bool canSleep = true;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true &&
					roomArray[i]->actorArray[j]->status.asleep < 999 &&
					roomArray[i]->actorArray[j]->status.friendly == false &&
					roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)
					canSleep = false;
			}
		}

		if (canSleep == true)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("A season passes...")));
			nextFloor = true;
			drawXRelative = 0;
			drawYRelative = 0;
			rumble = 0;
		}

		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I can't sleep!")));
	}

	else if (roomArray[roomNumber]->roomTileArray[roomTileNumber]->tileType == tileTypeTV)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Bzzt.")));//debug

		/*
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("It's the shopping channel,")));
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Order something?")));
		gameState = gameStateShopMenu;
		menuState = 0;
		drawXRelative = 0;
		drawYRelative = 0;
		*/
	}

	else if (roomArray[roomNumber]->roomTileArray[roomTileNumber]->tileType == tileTypeShippingContainer)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Ship crops?")));
		gameState = gameStateShipping;
		menuState = 0;
		drawXRelative = 0;
		drawYRelative = 0;
	}
}

void Player::attackGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->alive == true &&
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->watered == false)
		careForPlant(true, roomNumber, growingPlantNumber, roomArray, memoryTextArray);

	else
	{
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->takeDamage(1, 5);

		if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->exists == false)
		{
			std::string plantName = "plant";

			if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->grownCrop->identified == true)
				plantName = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->grownCrop->name + " plant";

			if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->alive == true &&
				roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->harvestable == true)
			{
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Harvested " + plantName + ".")));

				roomArray[roomNumber]->itemArray.push_back(std::make_unique<Item>(roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x, roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y, roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->grownCrop->subType, roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->strength));

				bool identified = false;

				for (int i = 0; i < identifiedPlants.size(); ++i)
				{
					if (roomArray[roomNumber]->itemArray[roomArray[roomNumber]->itemArray.size() - 1]->subType == identifiedPlants[i])//if i know this plant type, identify the dropped item
					{
						roomArray[roomNumber]->itemArray[roomArray[roomNumber]->itemArray.size() - 1]->identifyPlant(true);
						break;
					}
				}
			}

			else
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Plucked " + plantName + ".")));
		}
	}
}

void Player::attackEnemy(int& roomNumber, int& enemyNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	int damageDealt;
	
	if (equippedItem.get() != nullptr &&
		equippedItem->skill != noStatEquipSkill)
		damageDealt = generateAttackDamage(baseAttackPower + tempAttackPower + floor((double)equippedItem->strength / 2));
	else
		damageDealt = generateAttackDamage(baseAttackPower + tempAttackPower);

	if (status.shrunken >= true)
		damageDealt = floor((double)damageDealt / 2);

	if (damageDealt <= 0)
		damageDealt = 1;

	if (roomArray[roomNumber]->actorArray[enemyNumber]->status.asleep >= 999 ||
		roomArray[roomNumber]->actorArray[enemyNumber]->status.friendly == true)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("That wasn't very nice...")));
		trueMansWorld = false;
	}

	bool hitLanded = roomArray[roomNumber]->actorArray[enemyNumber]->takeDefendableDamage(damageDealt);

	if (hitLanded == false)
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Missed [" + roomArray[roomNumber]->actorArray[enemyNumber]->name + "]!")));
	else if (roomArray[roomNumber]->actorArray[enemyNumber]->status.exists == false)
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Calmed [" + roomArray[roomNumber]->actorArray[enemyNumber]->name + "]!")));
	else
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("[" + roomArray[roomNumber]->actorArray[enemyNumber]->name + "] took " + std::to_string(damageDealt) + " dmg!")));

	if (hitLanded == true &&
		roomArray[roomNumber]->actorArray[enemyNumber]->status.blocking >= true)
	{
		signed int moveX = 0;
		signed int moveY = 0;
		signed int targetMoveX = 0;
		signed int targetMoveY = 0;

		blockPushback(roomArray[roomNumber]->actorArray[enemyNumber]->x, roomArray[roomNumber]->actorArray[enemyNumber]->y, moveX, moveY, targetMoveX, targetMoveY, roomArray);

		if (moveX != 0 ||
			moveY != 0)
		{
			setLastXYOnMove(moveX, moveY);
			x += moveX;
			y += moveY;
			setGameViewAndDrawXY(gameView);
			onMove(false);
		}

		if (targetMoveX != 0 ||
			targetMoveY != 0)
		{
			roomArray[roomNumber]->actorArray[enemyNumber]->setLastXYOnMove(targetMoveX, targetMoveY);
			roomArray[roomNumber]->actorArray[enemyNumber]->x += targetMoveX;
			roomArray[roomNumber]->actorArray[enemyNumber]->y += targetMoveY;
		}
	}
}

bool Player::quickWait(std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (hungerSatiety == 0)
		return false;

	int myRoom = -1;

	for (int r = 0; r < roomArray.size(); ++r)
	{
		if (roomArray[r]->isInsideRoom(x, y) == true)
			myRoom = r;
	}

	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if(roomArray[i]->actorArray[j]->status.exists == true &&
				roomArray[i]->actorArray[j]->status.friendly == false &&
				roomArray[i]->actorArray[j]->status.asleep < 999)
			{
				if (roomArray[i]->actorArray[j]->isInThisDistanceOfMe(x, y, 1) == true ||
					myRoom > -1 &&
					roomArray[myRoom]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void Player::handleFacing()
{
	if (upLeftKey == true ||
		upKey == keyHeld &&
		leftKey == keyHeld)
	{
		facing = facingUpLeft;
	}

	else if (upRightKey == true ||
		upKey == keyHeld &&
		rightKey == keyHeld)
	{
		facing = facingUpRight;
	}

	else if (downLeftKey == true ||
		downKey == keyHeld &&
		leftKey == keyHeld)
	{
		facing = facingDownLeft;
	}

	else if (downRightKey == true ||
		downKey == keyHeld &&
		rightKey == keyHeld)
	{
		facing = facingDownRight;
	}

	if (diagonalKey == false)
	{
		if (upKey == true &&
			leftKey == false &&
			rightKey == false)
		{
			facing = facingUp;
		}

		else if (leftKey == true &&
			upKey == false &&
			downKey == false)
		{
			facing = facingLeft;
		}

		else if (downKey == true &&
			leftKey == false &&
			rightKey == false)
		{
			facing = facingDown;
		}

		else if (rightKey == true &&
			upKey == false &&
			downKey == false)
		{
			facing = facingRight;
		}
	}
}

void Player::useSlottedCropAbility(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	bool abilityActivated = false;

	if (equippedItem->slottedCrop->subType == bananaPlantType ||
		equippedItem->slottedCrop->subType == limePlantType)//any type where nothing happens
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Nothing happens.")));

	else if (equippedItem->slottedCrop->subType == carrotPlantType)//carrot
	{
		int returnX;
		int returnY;

		int teleportMe = roomArray[0]->getFreeSpace(returnX, returnY, x, y, false);

		if (teleportMe == true)
		{
			abilityActivated = true;
			x = returnX;
			y = returnY;
			setGameViewAndDrawXY(gameView);
			endTurn();
		}

		else if (teleportMe == -1)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I'm already home.")));
		else if (teleportMe = false)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("There's no space to teleport to.")));
	}

	else if (equippedItem->slottedCrop->subType == tomatoPlantType)//tomato
	{
		abilityActivated = true;
		status.seeAll = true;
	}

	else if (equippedItem->slottedCrop->subType == cucumberPlantType)//cucumber
	{
		//should start with the farthest enemies from playe otherwise theyll hit enemies that are about to move
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						cucumberMovesActor(i, j, roomArray);
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							cucumberMovesActor(i, j, roomArray);
							goto useSlottedAbilityCucumberContinue;
						}
					}
				}
			useSlottedAbilityCucumberContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == cherryPlantType)//cherry
	{
		abilityActivated = true;
		//*BOOM*

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;
				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == pX &&
							roomArray[i]->tileArray[j]->y == pY)
						{
							roomArray[i]->tileArray[j]->rumble = 5;
							goto useSlottedAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->x == pX &&
							roomArray[i]->roomTileArray[j]->y == pY)
						{
							roomArray[i]->roomTileArray[j]->rumble = 5;
							goto useSlottedAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == pX &&
							roomArray[i]->hallwayArray[j]->y == pY)
						{
							roomArray[i]->hallwayArray[j]->rumble = 5;
							goto useSlottedAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == pX &&
							roomArray[i]->growingPlantArray[j]->y == pY)
						{
							roomArray[i]->growingPlantArray[j]->exists = false;
							goto useSlottedAbilityCherryContinue;
						}
					}
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)
						{
							roomArray[i]->actorArray[j]->status.exists = false;
							goto useSlottedAbilityCherryContinue;
						}
					}
				}
			useSlottedAbilityCherryContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == applePlantType)//apple
	{
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						roomArray[i]->actorArray[j]->status.addStatusEffect(asleepStatusEffect, 999);
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							roomArray[i]->actorArray[j]->status.addStatusEffect(asleepStatusEffect, 999);
							goto useSlottedAbilityAppleContinue;
						}
					}
				}
			useSlottedAbilityAppleContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == orangePlantType)//orange
	{
		abilityActivated = true;

		int targetX = 0;
		int targetY = 0;
		targetFacing(targetX, targetY, false);

		if (isTileFree(x + targetX, y + targetY, roomArray) == true)
			roomArray[0]->actorArray.push_back(std::make_unique<Actor>(x + targetX, y + targetY, (actorTypeList)getRandomInt(0, maxActorTypeListSize - 1), false));
	}

	else if (equippedItem->slottedCrop->subType == eggplantPlantType)//eggplant
	{
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						roomArray[i]->actorArray[j]->status.addStatusEffect(poisonedStatusEffect, 999);
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							roomArray[i]->actorArray[j]->status.addStatusEffect(poisonedStatusEffect, 999);
							goto useSlottedAbilityEggplantContinue;
						}
					}
				}
			useSlottedAbilityEggplantContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == wheatPlantType)//wheat
	{
		abilityActivated = true;
		status.removeNegativeStatusEffects();
		status.resistStatusEffects = true;
	}

	else if (equippedItem->slottedCrop->subType == oatPlantType)//oat
	{
		abilityActivated = true;
		addHealth(maxHealth);
	}

	else if (equippedItem->slottedCrop->subType == redPepperPlantType)//red pepper
	{
		abilityActivated = true;
		int directionX;
		int directionY;
		targetFacing(directionX, directionY, true);

		int shootX = x;
		int shootY = y;

		bool hitSomething = false;

		while (hitSomething == false)
		{
			shootX += directionX;
			shootY += directionY;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
				{
					if (roomArray[i]->tileArray[j]->x == shootX &&
						roomArray[i]->tileArray[j]->y == shootY)
					{
						roomArray[i]->tileArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityRedPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
				{
					if (roomArray[i]->roomTileArray[j]->x == shootX &&
						roomArray[i]->roomTileArray[j]->y == shootY &&
						roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)
					{
						roomArray[i]->roomTileArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityRedPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
				{
					if (roomArray[i]->hallwayArray[j]->x == shootX &&
						roomArray[i]->hallwayArray[j]->y == shootY)
					{
						roomArray[i]->hallwayArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityRedPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
				{
					if (roomArray[i]->growingPlantArray[j]->exists == true &&
						roomArray[i]->growingPlantArray[j]->x == shootX &&
						roomArray[i]->growingPlantArray[j]->y == shootY)
					{
						roomArray[i]->growingPlantArray[j]->exists = false;
						hitSomething = true;
						goto useSlottedCropAbilityRedPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[i]->actorArray[j]->x == shootX &&
						roomArray[i]->actorArray[j]->y == shootY)
					{
						roomArray[i]->actorArray[j]->status.exists = false;
						hitSomething = true;
						goto useSlottedCropAbilityRedPepper;
					}
				}
			}

		useSlottedCropAbilityRedPepper:;
		}
	}

	else if (equippedItem->slottedCrop->subType == yellowPepperPlantType)//yellow pepper
	{
		abilityActivated = true;
		int directionX;
		int directionY;
		targetFacing(directionX, directionY, true);

		int shootX = x;
		int shootY = y;

		bool hitSomething = false;

		while (hitSomething == false)
		{
			shootX += directionX;
			shootY += directionY;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
				{
					if (roomArray[i]->tileArray[j]->x == shootX &&
						roomArray[i]->tileArray[j]->y == shootY)
					{
						roomArray[i]->tileArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityYellowPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
				{
					if (roomArray[i]->roomTileArray[j]->x == shootX &&
						roomArray[i]->roomTileArray[j]->y == shootY &&
						roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)
					{
						roomArray[i]->roomTileArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityYellowPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
				{
					if (roomArray[i]->hallwayArray[j]->x == shootX &&
						roomArray[i]->hallwayArray[j]->y == shootY)
					{
						roomArray[i]->hallwayArray[j]->rumble = 5;
						hitSomething = true;
						goto useSlottedCropAbilityYellowPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
				{
					if (roomArray[i]->growingPlantArray[j]->exists == true &&
						roomArray[i]->growingPlantArray[j]->x == shootX &&
						roomArray[i]->growingPlantArray[j]->y == shootY)
					{
						roomArray[i]->growingPlantArray[j]->takeDamage(1, 5);
						hitSomething = true;
						goto useSlottedCropAbilityYellowPepper;
					}
				}

				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[i]->actorArray[j]->x == shootX &&
						roomArray[i]->actorArray[j]->y == shootY)
					{
						roomArray[i]->actorArray[j]->status.addStatusEffect(stunnedStatusEffect, 999);
						hitSomething = true;
						goto useSlottedCropAbilityYellowPepper;
					}
				}
			}

		useSlottedCropAbilityYellowPepper:;
		}
	}

	else if (equippedItem->slottedCrop->subType == lemonPlantType)//lemon
	{
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						if (roomArray[i]->actorArray[j]->devolveType != evolveTypeNone)
						{
							roomArray[i]->actorArray[j]->actorType = roomArray[i]->actorArray[j]->devolveType;
							roomArray[i]->actorArray[j]->setType();
						}
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							if (roomArray[i]->actorArray[j]->devolveType != evolveTypeNone)
							{
								roomArray[i]->actorArray[j]->actorType = roomArray[i]->actorArray[j]->devolveType;
								roomArray[i]->actorArray[j]->setType();
							}

							goto useSlottedAbilityLemonContinue;
						}
					}
				}
			useSlottedAbilityLemonContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == carambolaPlantType)//carambola
	{
		abilityActivated = true;
		status.addStatusEffect(stopTimeStatusEffect, 5);
	}

	else if (equippedItem->slottedCrop->subType == cornPlantType)//corn
	{
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						roomArray[i]->actorArray[j]->status.addStatusEffect(shrunkenStatusEffect, 999);
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							roomArray[i]->actorArray[j]->status.addStatusEffect(shrunkenStatusEffect, 999);
							goto useSlottedAbilityCornContinue;
						}
					}
				}
			useSlottedAbilityCornContinue:;
			}
		}
	}

	else if (equippedItem->slottedCrop->subType == grapePlantType)//grape
	{
		abilityActivated = true;
		int roomNumber = -1;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInRoomAndDoorway(x, y) == true)//is player in room i
			{
				roomNumber = i;
				break;
			}
		}

		if (roomNumber >= 0)
		{
			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
					{
						roomArray[i]->actorArray[j]->status.addStatusEffect(confuseStatusEffect, 999);
					}
				}
			}
		}

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)//is actor in 1x1 range of player
						{
							roomArray[i]->actorArray[j]->status.addStatusEffect(confuseStatusEffect, 999);
							goto useSlottedAbilityGrapeContinue;
						}
					}
				}
			useSlottedAbilityGrapeContinue:;
			}
		}
	}

	if (abilityActivated == true)
		equippedItem->powerSave(memoryTextArray);
}

void Player::cucumberMovesActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray)
{
	int targetX;
	int targetY;
	roomArray[roomNumber]->actorArray[actorNumber]->setFacing(x, y, false);//faces towards player
	roomArray[roomNumber]->actorArray[actorNumber]->targetFacing(targetX, targetY, false);//returns the direction of player
	targetX = -targetX;//returns the opposite of faced direction
	targetY = -targetY;

	while (isTileFree(roomArray[roomNumber]->actorArray[actorNumber]->x + targetX, roomArray[roomNumber]->actorArray[actorNumber]->y + targetY, roomArray) == true)//while the target tile is free
	{
		if (roomArray[roomNumber]->actorArray[actorNumber]->x + targetX == x &&
			roomArray[roomNumber]->actorArray[actorNumber]->y + targetY == y)//break if target tile == player
			break;

		roomArray[roomNumber]->actorArray[actorNumber]->x += targetX;//move to target tile
		roomArray[roomNumber]->actorArray[actorNumber]->y += targetY;
	}
}

void Player::identifyMenuState(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (inventory[menuState]->itemType == seedItemType ||
		inventory[menuState]->itemType == cropItemType)
	{
		for (int j = 0; j < identifiedPlants.size(); ++j)
		{
			if (inventory[menuState]->subType == identifiedPlants[j])
				return;
		}

		identifiedPlants.push_back((plantTypeList)inventory[menuState]->subType);
		recalculatePlantIdentification(roomArray);
	}

	else if (inventory[menuState]->itemType == herbItemType)
	{
		for (int j = 0; j < identifiedHerbs.size(); ++j)
		{
			if (inventory[menuState]->subType == identifiedHerbs[j])
				return;
		}

		identifiedHerbs.push_back((herbTypeList)inventory[menuState]->subType);
		recalculateHerbIdentification(roomArray);
	}

	bool identifiedIdentifyHerb = false;

	for (int j = 0; j < identifiedHerbs.size(); ++j)//identify identify herb if it isnt already
	{
		if (identifyHerbType == identifiedHerbs[j])
			identifiedIdentifyHerb = true;
	}

	if (identifiedIdentifyHerb == false)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("It was [Identify] Herb!")));
		identifiedHerbs.push_back(identifyHerbType);
		recalculateHerbIdentification(roomArray);
	}

	memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Identified item!")));
}

void Player::destroyMenuState(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	bool identifiedDestroyHerb = false;

	for (int j = 0; j < identifiedHerbs.size(); ++j)//identify destroy herb if it isnt already
	{
		if (destroyHerbType == identifiedHerbs[j])
			identifiedDestroyHerb = true;
	}

	if (identifiedDestroyHerb == false)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("It was [Destroy] Herb!")));
		identifiedHerbs.push_back(destroyHerbType);
		recalculateHerbIdentification(roomArray);
	}

	memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Destroyed item...")));
	removeMenuStateFromInventory();
}

void Player::useItem(int& season, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	if (inventory[menuState]->itemType == seedItemType)
	{
		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInsideRoom(x, y) == true)
			{
				if (i == 0)//if in house
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant inside!")));
					gameState = gameStateIngame;
					return;
				}

				int plantingX;
				int plantingY;

				if (getFreePlantingSpace(plantingX, plantingY, i, season, roomArray, memoryTextArray) == true)
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Planted " + inventory[menuState]->name + inventory[menuState]->addName + ".")));
					roomArray[i]->growingPlantArray.push_back(std::make_unique<GrowingPlant>(plantingX, plantingY, (plantTypeList)inventory[menuState]->subType, inventory[menuState]->identified));

					if (equippedItem.get() != nullptr)
					{
						if (equippedItem->skill == plantMasteryEquipSkill)
							roomArray[i]->growingPlantArray[roomArray[i]->growingPlantArray.size() - 1]->growthLimit = 1000;
						else if (equippedItem->skill == fastGrowEquipSkill)
							roomArray[i]->growingPlantArray[roomArray[i]->growingPlantArray.size() - 1]->growthSpeed = 2;
						else if (equippedItem->skill == plantHPUpEquipSkill)
							roomArray[i]->growingPlantArray[roomArray[i]->growingPlantArray.size() - 1]->health = 4;
					}

					if (inventory[menuState]->doubled == false &&
						equippedItem.get() != nullptr &&
						equippedItem->skill == seedSaveEquipSkill &&
						getRandomInt(1, 100) <= equippedItem->skillChance)
					{
						memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Seed doubled!")));
						inventory[menuState]->doubled = true;
					}

					else
						removeMenuStateFromInventory();

					endTurn();
					return;
				}

				else
				{
					gameState = gameStateIngame;
					return;
				}

				return;
			}
		}

		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));
	}

	else if (inventory[menuState]->itemType == cropItemType)
	{
		if (inventory[menuState]->identified == false)
		{
			identifiedPlants.push_back((plantTypeList)inventory[menuState]->subType);
			recalculatePlantIdentification(roomArray);
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Identified " + inventory[menuState]->name + "!")));
		}

		bool deleteObject = true;
		
		int eatenNutrition = inventory[menuState]->nutrition;

		if (equippedItem.get() != nullptr &&
			equippedItem->skill == cropFoodUpEquipSkill)
			eatenNutrition += 10;

		eatFood(eatenNutrition, inventory[menuState]->name, memoryTextArray);

		useEatenCropAbility(roomArray, memoryTextArray, gameView);

		if (inventory[menuState]->strength == 3)//s grade plants fully heal when eaten
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Delicious!")));
			addHealth(maxHealth);
		}
		
		if (inventory[menuState]->doubled == false &&
			equippedItem.get() != nullptr &&
			equippedItem->skill == cropSaveEquipSkill &&
			getRandomInt(1, 100) <= equippedItem->skillChance)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Crop doubled!")));
			inventory[menuState]->doubled = true;
			deleteObject = false;
		}

		if (equippedItem.get() != nullptr &&
			equippedItem->slottedCrop.get() == nullptr &&
			inventory[menuState]->strength >= 2)//slot it in equippedItem
		{
			//*chk-kaching!*
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(inventory[menuState]->name + " slotted!")));

			if (inventory[menuState]->strength == 2)
				equippedItem->strength++;

			else if (inventory[menuState]->strength == 3)
				equippedItem->strength += 2;

			if (deleteObject == true)
				equippedItem->slottedCrop = std::move(inventory[menuState]);
			else
				equippedItem->slottedCrop = std::make_unique<Item>((itemTypeList)inventory[menuState]->itemType, inventory[menuState]->subType);
		}

		else if (equippedItem.get() != nullptr &&
			equippedItem->slottedCrop.get() != nullptr &&
			equippedItem->slottedCrop->subType == inventory[menuState]->subType &&
			equippedItem->strength < 4)//strengthen equippedItem
		{
			if (inventory[menuState]->strength == 2)
			{
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(equippedItem->name + " powered up!")));
				equippedItem->strength++;
			}

			else if (inventory[menuState]->strength == 3)
			{
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(equippedItem->name + " super powered!")));
				equippedItem->strength += 2;

				if (equippedItem->strength > 4)
					equippedItem->strength = 4;
			}
		}

		if (deleteObject == true)
			removeMenuStateFromInventory();

		endTurn();
		return;
	}

	else if (inventory[menuState]->itemType == herbItemType)
	{
		if (inventory[menuState]->subType == tastyHerbType)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Tasty!")));

		else if (inventory[menuState]->subType == hungryHerbType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I'm hungry...")));
			status.fastHunger = true;
		}

		else if (inventory[menuState]->subType == healHerbType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Healed!")));
			addHealth(5);
		}

		else if (inventory[menuState]->subType == hurtHerbType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Ouch!")));
			takeTrueDamage(5);
		}

		else if (inventory[menuState]->subType == identifyHerbType)
			gameState = gameStateIdentifyItem;

		else if (inventory[menuState]->subType == destroyHerbType)
			gameState = gameStateDestroyItem;

		else if (inventory[menuState]->subType == rememberHerbType)
			rememberHerbPower(roomArray, memoryTextArray);

		else if (inventory[menuState]->subType == forgetHerbType)
			forgetHerbPower(roomArray, memoryTextArray);

		if (equippedItem.get() != nullptr &&
			equippedItem->skill == herbFoodUpEquipSkill)
			inventory[menuState]->nutrition = 20;

		if (gameState != gameStateIdentifyItem &&
			gameState != gameStateDestroyItem)
		{
			if (inventory[menuState]->identified == false)
			{
				identifiedHerbs.push_back((herbTypeList)inventory[menuState]->subType);
				recalculateHerbIdentification(roomArray);
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Identified " + inventory[menuState]->name + inventory[menuState]->addName + "!")));
			}
		}

		eatFood(inventory[menuState]->nutrition, inventory[menuState]->name + inventory[menuState]->addName, memoryTextArray);

		if (inventory[menuState]->doubled == false &&
			equippedItem.get() != nullptr &&
			equippedItem->skill == itemSaveEquipSkill &&
			getRandomInt(1, 100) <= equippedItem->skillChance)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Item doubled!")));
			inventory[menuState]->doubled = true;
		}

		else
			removeMenuStateFromInventory();

		if (gameState != gameStateIdentifyItem &&
			gameState != gameStateDestroyItem)
			endTurn();
		return;
	}
}

int Player::getFreePlantingSpace(int& returnX, int& returnY, int& currentRoom, int& season, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int targetX = 0;
	int targetY = 0;
	targetFacing(targetX, targetY, false);

	if (season == seasonWinter)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("It's too cold to farm!")));
		return false;
	}

	for (int i = 0; i < roomArray[currentRoom]->growingPlantArray.size(); ++i)
	{
		if (roomArray[currentRoom]->growingPlantArray[i]->exists == true)//dont allow planting in rooms with growingPlants
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("This field is already farmed!")));
			return false;
		}
	}

	if (x + targetX < roomArray[currentRoom]->roomSize.x + 1 ||//dont place in doorways
		x + targetX > roomArray[currentRoom]->roomSize.x + roomArray[currentRoom]->roomSize.w - 1 ||
		y + targetY< roomArray[currentRoom]->roomSize.y + 1 ||
		y + targetY > roomArray[currentRoom]->roomSize.y + roomArray[currentRoom]->roomSize.h - 1)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));
		return false;
	}

	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (roomArray[i]->actorArray[j]->status.exists == true)
			{
				if (roomArray[i]->actorArray[j]->x == x + targetX &&
					roomArray[i]->actorArray[j]->y == y + targetY)//if actor in the way
				{
					memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));
					return false;
				}
			}
		}
	}

	for (int i = 0; i < roomArray[currentRoom]->tileArray.size(); ++i)
	{
		if (roomArray[currentRoom]->tileArray[i]->x == x + targetX &&
			roomArray[currentRoom]->tileArray[i]->y == y + targetY)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));
			return false;
		}
	}

	for (int i = 0; i < roomArray[currentRoom]->roomTileArray.size(); ++i)
	{
		if (roomArray[currentRoom]->roomTileArray[i]->x == x + targetX &&
			roomArray[currentRoom]->roomTileArray[i]->y == y + targetY)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));

			if (roomArray[currentRoom]->roomTileArray[i]->tileType == tileTypeTrap)
			{
				roomArray[currentRoom]->roomTileArray[i]->visible = true;
				roomArray[currentRoom]->roomTileArray[i]->discovered = true;
			}

			return false;
		}
	}

	for (int i = 0; i < roomArray[currentRoom]->itemArray.size(); ++i)
	{
		if (roomArray[currentRoom]->itemArray[i]->x == x + targetX &&
			roomArray[currentRoom]->itemArray[i]->y == y + targetY)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Can't plant here!")));
			return false;
		}
	}

	animateAttackMove(targetX, targetY);//animate planting
	returnX = x + targetX;
	returnY = y + targetY;

	return true;//free space found
}

void Player::useEatenCropAbility(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray, SDL_Rect& gameView)
{
	if (inventory[menuState]->subType == carrotPlantType)//carrot
	{
		int returnX;
		int returnY;
		int teleportMe = -1;

		if (roomArray.size() > 1)
		{
			teleportMe = roomArray[getRandomInt(0, roomArray.size() - 1)]->getFreeSpace(returnX, returnY, x, y, true);

			if (teleportMe == true)//just doesnt teleport you if it picks a full room
			{
				x = returnX;
				y = returnY;
				setGameViewAndDrawXY(gameView);
				endTurn();
			}
		}
	}
	
	else if (inventory[menuState]->subType == tomatoPlantType)//tomato
	{
		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap)
					roomArray[i]->roomTileArray[j]->discovered = true;
			}
		}
	}

	else if (inventory[menuState]->subType == cucumberPlantType)//cucumber
	{
		int targetX;
		int targetY;
		targetFacing(targetX, targetY, false);//returns the faced direction
		targetX = -targetX;//returns the opposite of faced direction
		targetY = -targetY;

		while (isTileFree(x + targetX, y + targetY, roomArray) == true)//while the target tile is free
		{
			x += targetX;//move to target tile
			y += targetY;
			setGameViewAndDrawXY(gameView);
			onMove(false);
		}
	}

	else if (inventory[menuState]->subType == cherryPlantType)//cherry
	{
		//*BOOM*
		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;
				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == pX &&
							roomArray[i]->tileArray[j]->y == pY)
						{
							roomArray[i]->tileArray[j]->rumble = 5;
							goto useEatenCropAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->x == pX &&
							roomArray[i]->roomTileArray[j]->y == pY)
						{
							roomArray[i]->roomTileArray[j]->rumble = 5;
							goto useEatenCropAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == pX &&
							roomArray[i]->hallwayArray[j]->y == pY)
						{
							roomArray[i]->hallwayArray[j]->rumble = 5;
							goto useEatenCropAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == pX &&
							roomArray[i]->growingPlantArray[j]->y == pY)
						{
							roomArray[i]->growingPlantArray[j]->exists = false;
							goto useEatenCropAbilityCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)
						{
							roomArray[i]->actorArray[j]->status.exists = false;
							goto useEatenCropAbilityCherryContinue;
						}
					}
				}
			useEatenCropAbilityCherryContinue:;
			}
		}
		
		takeTrueDamage(ceil((double)health / 2));
	}

	else if (inventory[menuState]->subType == bananaPlantType)//banana
	{
		if (roomArray.size() > 0)
		{
			int randomRoom = getRandomInt(0, roomArray.size() - 1);
			int randomX;
			int randomY;

			if (roomArray[randomRoom]->getFreeSpace(randomX, randomY, x, y, true) == true)
				roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}
	}

	else if (inventory[menuState]->subType == applePlantType)//apple
		status.addStatusEffect(asleepStatusEffect, 10);

	else if (inventory[menuState]->subType == orangePlantType)//orange
	{
		std::vector<SDL_Rect> freeSpaceArray;
		int summonX;
		int summonY;
		std::vector<std::pair<int, int>> summonEnemyArray;

		for (int pX = x - 1; pX <= x + 1; ++pX)
		{
			for (int pY = y - 1; pY <= y + 1; ++pY)
			{
				if (pX == x &&
					pY == y)
					continue;
				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == pX &&
							roomArray[i]->tileArray[j]->y == pY)
						{
							goto useEatenCropAbilityOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap &&
							roomArray[i]->roomTileArray[j]->x == pX &&
							roomArray[i]->roomTileArray[j]->y == pY)
						{
							goto useEatenCropAbilityOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == pX &&
							roomArray[i]->hallwayArray[j]->y == pY)
						{
							goto useEatenCropAbilityOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == pX &&
							roomArray[i]->growingPlantArray[j]->y == pY)
						{
							goto useEatenCropAbilityOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == pX &&
							roomArray[i]->actorArray[j]->y == pY)
						{
							goto useEatenCropAbilityOrangeContinue;
						}
					}
				}

				freeSpaceArray.push_back(SDL_Rect{ pX, pY, 0, 0 });

			useEatenCropAbilityOrangeContinue:;
			}
		}

		if (freeSpaceArray.size() > 0)//if theres space next to you
		{
			int randomFreeSpace = getRandomInt(0, freeSpaceArray.size() - 1);
			summonX = freeSpaceArray[randomFreeSpace].x;
			summonY = freeSpaceArray[randomFreeSpace].y;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true)
						summonEnemyArray.push_back(std::make_pair(i, j));
				}
			}

			if (summonEnemyArray.size() > 0)//if there are enemies to summon
			{
				int randomSummon = getRandomInt(0, summonEnemyArray.size() - 1);
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->x = summonX;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->y = summonY;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->setDrawXY();
			}
		}
	}

	else if (inventory[menuState]->subType == eggplantPlantType)//eggplant
		status.addStatusEffect(poisonedStatusEffect, 10);

	else if (inventory[menuState]->subType == wheatPlantType)//wheat
		status.removeNegativeStatusEffects();

	else if (inventory[menuState]->subType == oatPlantType)//oat
		addHealth(maxHealth);

	else if (inventory[menuState]->subType == redPepperPlantType)//red pepper
		takeTrueDamage(5);

	else if (inventory[menuState]->subType == yellowPepperPlantType)//yellow pepper
		status.addStatusEffect(stunnedStatusEffect, 10);

	else if (inventory[menuState]->subType == lemonPlantType)//lemon
	{
		std::vector<std::pair<int, int>> evolveEnemyArray;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true)
					evolveEnemyArray.push_back(std::make_pair(i, j));
			}
		}

		if (evolveEnemyArray.size() > 0)
		{
			int random = getRandomInt(0, evolveEnemyArray.size() - 1);

			if (roomArray[evolveEnemyArray[random].first]->actorArray[evolveEnemyArray[random].second]->status.asleep > 0)
			{
				int sleepyness = roomArray[evolveEnemyArray[random].first]->actorArray[evolveEnemyArray[random].second]->status.asleep;
				roomArray[evolveEnemyArray[random].first]->actorArray[evolveEnemyArray[random].second]->powerUp(memoryTextArray);
				roomArray[evolveEnemyArray[random].first]->actorArray[evolveEnemyArray[random].second]->status.asleep = sleepyness;
			}
			
			else
				roomArray[evolveEnemyArray[random].first]->actorArray[evolveEnemyArray[random].second]->powerUp(memoryTextArray);
		}
	}

	else if (inventory[menuState]->subType == limePlantType)//lime
	{
		std::vector<std::pair<int, int>> sleepingEnemyArray;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true &&
					roomArray[i]->actorArray[j]->status.asleep >= true)
				{
					sleepingEnemyArray.push_back(std::make_pair(i, j));
				}
			}
		}

		if (sleepingEnemyArray.size() > 0)
		{
			int random = getRandomInt(0, sleepingEnemyArray.size() - 1);
			roomArray[sleepingEnemyArray[random].first]->actorArray[sleepingEnemyArray[random].second]->status.asleep = false;
		}
	}

	else if (inventory[menuState]->subType == carambolaPlantType)//carambola
		status.addStatusEffect(stopTimeStatusEffect, 2);

	else if (inventory[menuState]->subType == cornPlantType)//corn
		status.addStatusEffect(shrunkenStatusEffect, 10);

	else if (inventory[menuState]->subType == grapePlantType)//grape
		status.addStatusEffect(confuseStatusEffect, 10);
}

void Player::rememberHerbPower(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	bool learned = false;//true if i learned something or i've already learned everything
	int getRandomType = getRandomInt(1, 2);
	std::vector<int> subTypeList;

	bool plantsIdentified = false;
	bool herbsIdentified = false;

	if (identifiedPlants.size() == maxPlantTypeListSize)
		plantsIdentified = true;
	if (identifiedHerbs.size() == maxHerbTypeListSize)
		herbsIdentified = true;

	if (plantsIdentified == true &&
		herbsIdentified == true)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Remembered something!")));
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...But I know everything!")));
		return;
	}

	else
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Remembered something!")));

	while (learned == false)
	{
		if (getRandomType == 1 &&
			plantsIdentified == false ||
			getRandomType == 2 &&
			herbsIdentified == true)
		{
			for (int i = 0; i < maxPlantTypeListSize; ++i)//this is the better way of doing learnHerb, rather than the others
			{
				for (int j = 0; j < identifiedPlants.size(); ++j)
				{
					if (identifiedPlants[j] == (plantTypeList)i)//i already know this one, ignore it
						goto identifyPlantContinue;
				}

				subTypeList.push_back(i);

			identifyPlantContinue:;
			}

			if (subTypeList.size() > 0)
			{
				identifiedPlants.push_back((plantTypeList)subTypeList[getRandomInt(0, subTypeList.size() - 1)]);
				recalculatePlantIdentification(roomArray);
				learned = true;
			}
		}

		else if (getRandomType == 2 &&
			herbsIdentified == false ||
			getRandomType == 1 &&
			plantsIdentified == true)
		{
			for (int i = 0; i < maxHerbTypeListSize; ++i)
			{
				for (int j = 0; j < identifiedHerbs.size(); ++j)
				{
					if (identifiedHerbs[j] == (herbTypeList)i)//i already know this one, ignore it
						goto identifyHerbContinue;
				}

				subTypeList.push_back(i);

			identifyHerbContinue:;
			}

			if (subTypeList.size() > 0)
			{
				identifiedHerbs.push_back((herbTypeList)subTypeList[getRandomInt(0, subTypeList.size() - 1)]);
				recalculateHerbIdentification(roomArray);
				learned = true;
			}
		}
	}
}

void Player::forgetHerbPower(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	bool forgot = false;
	int getRandomType = getRandomInt(1, 2);

	while (forgot == false)
	{
		if (identifiedPlants.size() > 0 &&
			getRandomType == 1||
			identifiedPlants.size() > 0 &&
			getRandomType == 2 &&
			identifiedHerbs.size() == 0)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Forgot something...")));
			identifiedPlants.erase(identifiedPlants.begin() + getRandomInt(0, identifiedPlants.size() - 1));
			recalculatePlantIdentification(roomArray);
			forgot = true;
		}

		else if (identifiedHerbs.size() > 0 &&
			getRandomType == 2 ||
			identifiedHerbs.size() > 0 &&
			getRandomType == 1 &&
			identifiedPlants.size() == 0)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Forgot something...")));
			identifiedHerbs.erase(identifiedHerbs.begin() + getRandomInt(0, identifiedHerbs.size() - 1));
			recalculateHerbIdentification(roomArray);
			forgot = true;
		}

		else
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Forgot something...")));
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...But I don't know anything.")));
			forgot = true;
		}
	}
}

void Player::throwItem(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int directionX;
	int directionY;
	targetFacing(directionX, directionY, true);

	int throwX = x;
	int throwY = y;
	int nextTurnThrowX = throwX;
	int nextTurnThrowY = throwY;

	bool hitSomething = false;

	while (hitSomething == false)
	{
		nextTurnThrowX += directionX;
		nextTurnThrowY += directionY;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
			{
				if (roomArray[i]->tileArray[j]->x == nextTurnThrowX &&
					roomArray[i]->tileArray[j]->y == nextTurnThrowY)
				{
					roomArray[i]->tileArray[j]->rumble = 5;
					hitSomething = true;

					int dropX = throwX;
					int dropY = throwY;
					int returnRoom = -1;

					if (getFreeDroppingSpace(dropX, dropY, returnRoom, roomArray) == true)
					{
						inventory[menuState]->x = dropX;
						inventory[menuState]->y = dropY;
						inventory[menuState]->lastX = x;
						inventory[menuState]->lastY = y;
						inventory[menuState]->drawX = inventory[menuState]->lastX * inventory[menuState]->spriteSheetRect.w;
						inventory[menuState]->drawY = inventory[menuState]->lastY * inventory[menuState]->spriteSheetRect.h;
						roomArray[returnRoom]->itemArray.push_back(std::move(inventory[menuState]));

						if(x == dropX &&
							y == dropY)
							standingOnObject = true;
						endTurn();
					}

					goto throwItemBreak;
				}
			}

			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (roomArray[i]->roomTileArray[j]->x == nextTurnThrowX &&
					roomArray[i]->roomTileArray[j]->y == nextTurnThrowY &&
					roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)
				{
					roomArray[i]->roomTileArray[j]->rumble = 5;
					hitSomething = true;
					
					int dropX = throwX;
					int dropY = throwY;
					int returnRoom = -1;

					if (getFreeDroppingSpace(dropX, dropY, returnRoom, roomArray) == true)
					{
						inventory[menuState]->x = dropX;
						inventory[menuState]->y = dropY;
						inventory[menuState]->setDrawXY();
						roomArray[returnRoom]->itemArray.push_back(std::move(inventory[menuState]));

						if (x == dropX &&
							y == dropY)
							standingOnObject = true;
						endTurn();
					}

					goto throwItemBreak;
				}
			}

			for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
			{
				if (roomArray[i]->hallwayArray[j]->x == nextTurnThrowX &&
					roomArray[i]->hallwayArray[j]->y == nextTurnThrowY)
				{
					roomArray[i]->hallwayArray[j]->rumble = 5;
					hitSomething = true;
					
					int dropX = throwX;
					int dropY = throwY;
					int returnRoom = -1;

					if (getFreeDroppingSpace(dropX, dropY, returnRoom, roomArray) == true)
					{
						inventory[menuState]->x = dropX;
						inventory[menuState]->y = dropY;
						inventory[menuState]->setDrawXY();
						roomArray[returnRoom]->itemArray.push_back(std::move(inventory[menuState]));

						if (x == dropX &&
							y == dropY)
							standingOnObject = true;
						endTurn();
					}

					goto throwItemBreak;
				}
			}

			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->exists == true &&
					roomArray[i]->growingPlantArray[j]->x == nextTurnThrowX &&
					roomArray[i]->growingPlantArray[j]->y == nextTurnThrowY)
				{
					throwHitsGrowingPlant(i, j, roomArray, memoryTextArray);
					hitSomething = true;
					goto throwItemBreak;
				}
			}

			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true &&
					roomArray[i]->actorArray[j]->x == nextTurnThrowX &&
					roomArray[i]->actorArray[j]->y == nextTurnThrowY)
				{
					throwHitsActor(i, j, roomArray, memoryTextArray);
					hitSomething = true;
					goto throwItemBreak;
				}
			}
		}

		throwX = nextTurnThrowX;
		throwY = nextTurnThrowY;

	throwItemBreak:;
	}

	removeMenuStateFromInventory();
}

void Player::throwHitsGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (inventory[menuState]->itemType == cropItemType)
		throwCropAffectsGrowingPlant(roomNumber, growingPlantNumber, roomArray, memoryTextArray);

	if (inventory[menuState]->itemType == herbItemType ||
		inventory[menuState]->itemType == cropItemType)
		careForPlant(false, roomNumber, growingPlantNumber, roomArray, memoryTextArray);
	else
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->takeDamage(1, 5);
}

void Player::throwCropAffectsGrowingPlant(int& roomNumber, int& growingPlantNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (inventory[menuState]->subType == carrotPlantType)//carrot
	{
		int returnRoom = getRandomInt(0, roomArray.size() - 1);
		int returnX;
		int returnY;
		int teleportMe = -1;

		if (roomArray.size() > 1)
		{
			teleportMe = roomArray[returnRoom]->getFreeSpace(returnX, returnY, x, y, true);

			if (teleportMe == true)//just doesnt teleport you if it picks a full room
			{
				roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x = returnX;
				roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y = returnY;
				roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->setDrawXY();
				roomArray[returnRoom]->growingPlantArray.push_back(std::move(roomArray[roomNumber]->growingPlantArray[growingPlantNumber]));
				roomArray[roomNumber]->growingPlantArray.erase(roomArray[roomNumber]->growingPlantArray.begin() + growingPlantNumber);

				roomNumber = returnRoom;
				growingPlantNumber = roomArray[returnRoom]->growingPlantArray.size() - 1;
			}
		}
	}

	else if (inventory[menuState]->subType == cherryPlantType)//cherry
	{
		//*BOOM*
		for (int eX = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x - 1; eX <= roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x + 1; ++eX)
		{
			for (int eY = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y - 1; eY <= roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y + 1; ++eY)
			{
				if (eX == roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x &&
					eY == roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y)
					continue;

				if (eX == x &&
					eY == y)
				{
					takeTrueDamage(ceil((double)health / 2));
					continue;
				}

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == eX &&
							roomArray[i]->tileArray[j]->y == eY)
						{
							roomArray[i]->tileArray[j]->rumble = 5;
							goto cropAffectsGrowingPlantCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->x == eX &&
							roomArray[i]->roomTileArray[j]->y == eY)
						{
							roomArray[i]->roomTileArray[j]->rumble = 5;
							goto cropAffectsGrowingPlantCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == eX &&
							roomArray[i]->hallwayArray[j]->y == eY)
						{
							roomArray[i]->hallwayArray[j]->rumble = 5;
							goto cropAffectsGrowingPlantCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == eX &&
							roomArray[i]->growingPlantArray[j]->y == eY)
						{
							roomArray[i]->growingPlantArray[j]->exists = false;
							goto cropAffectsGrowingPlantCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == eX &&
							roomArray[i]->actorArray[j]->y == eY)
						{
							roomArray[i]->actorArray[j]->status.exists = false;
							goto cropAffectsGrowingPlantCherryContinue;
						}
					}
				}
			cropAffectsGrowingPlantCherryContinue:;
			}
		}

		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->exists = false;//explodes plant too
	}

	else if (inventory[menuState]->subType == bananaPlantType)//banana
	{
		if (roomArray.size() > 0)
		{
			int randomRoom = getRandomInt(0, roomArray.size() - 1);
			int randomX;
			int randomY;

			if (roomArray[randomRoom]->getFreeSpace(randomX, randomY, x, y, true) == true)
				roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}
	}

	else if (inventory[menuState]->subType == orangePlantType)//orange
	{
		std::vector<SDL_Rect> freeSpaceArray;
		int summonX;
		int summonY;
		std::vector<std::pair<int, int>> summonEnemyArray;

		for (int eX = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x - 1; eX <= roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x + 1; ++eX)
		{
			for (int eY = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y - 1; eY <= roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y + 1; ++eY)
			{
				if (eX == roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->x &&
					eY == roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == eX &&
							roomArray[i]->tileArray[j]->y == eY)
						{
							goto cropAffectsGrowingPlantOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap &&
							roomArray[i]->roomTileArray[j]->x == eX &&
							roomArray[i]->roomTileArray[j]->y == eY)
						{
							goto cropAffectsGrowingPlantOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == eX &&
							roomArray[i]->hallwayArray[j]->y == eY)
						{
							goto cropAffectsGrowingPlantOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == eX &&
							roomArray[i]->growingPlantArray[j]->y == eY)
						{
							goto cropAffectsGrowingPlantOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == eX &&
							roomArray[i]->actorArray[j]->y == eY)
						{
							goto cropAffectsGrowingPlantOrangeContinue;
						}
					}
				}

				freeSpaceArray.push_back(SDL_Rect{ eX, eY, 0, 0 });

			cropAffectsGrowingPlantOrangeContinue:;
			}
		}

		if (freeSpaceArray.size() > 0)//if theres space next to you
		{
			int randomFreeSpace = getRandomInt(0, freeSpaceArray.size() - 1);
			summonX = freeSpaceArray[randomFreeSpace].x;
			summonY = freeSpaceArray[randomFreeSpace].y;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true)
						summonEnemyArray.push_back(std::make_pair(i, j));
				}
			}


			if (summonEnemyArray.size() > 0)//if there are enemies to summon
			{
				int randomSummon = getRandomInt(0, summonEnemyArray.size() - 1);
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->x = summonX;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->y = summonY;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->setDrawXY();
			}
		}
	}

	else if (inventory[menuState]->subType == eggplantPlantType)//eggplant
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->wither();

	else if (inventory[menuState]->subType == oatPlantType)//oat
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->health = roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->maxHealth;

	else if (inventory[menuState]->subType == redPepperPlantType)//red pepper
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->exists = false;

	else if (inventory[menuState]->subType == lemonPlantType)//lemon
	{
		if (roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->stage < 4)
		{
			roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->growthTimer = 500;
			roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->stage = 4;
		}

		else
		{
			roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->growthTimer = 1000;
			roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->stage = 5;
		}
	}

	else if (inventory[menuState]->subType == cornPlantType)//corn
	{
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->growthTimer = 0;
		roomArray[roomNumber]->growingPlantArray[growingPlantNumber]->stage = 0;
	}

}

void Player::throwHitsActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (roomArray[roomNumber]->actorArray[actorNumber]->status.exists == true)
	{
		roomArray[roomNumber]->actorArray[actorNumber]->takeTrueDamage(5);

		if (inventory[menuState]->itemType == cropItemType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Fed [" + roomArray[roomNumber]->actorArray[actorNumber]->name + "] crop!")));

			thrownCropAffectsActor(roomNumber, actorNumber, roomArray, memoryTextArray);

			int friendlyChance = (inventory[menuState]->strength + 1) * 25;

			if (trueMansWorld == true)
				friendlyChance += 5;

			if (roomArray[roomNumber]->actorArray[actorNumber]->status.exists == true &&
				roomArray[roomNumber]->actorArray[actorNumber]->status.friendly == false &&
				getRandomInt(1, 100) <= friendlyChance)
			{
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("[" + roomArray[roomNumber]->actorArray[actorNumber]->name + "] became tame!")));
				roomArray[roomNumber]->actorArray[actorNumber]->status.friendly = true;
			}
		}

		if (roomArray[roomNumber]->actorArray[actorNumber]->status.exists == false)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Calmed [" + roomArray[roomNumber]->actorArray[actorNumber]->name + "]!")));
		else
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("[" + roomArray[roomNumber]->actorArray[actorNumber]->name + "] took 5 dmg")));
	}
}

void Player::thrownCropAffectsActor(int& roomNumber, int& actorNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	roomArray[roomNumber]->actorArray[actorNumber]->status.asleep = false;
	roomArray[roomNumber]->actorArray[actorNumber]->rumble = 5;

	if (inventory[menuState]->subType == carrotPlantType)//carrot
	{
		int returnX;
		int returnY;
		int teleportMe = -1;

		if (roomArray.size() > 1)
		{
			teleportMe = roomArray[getRandomInt(0, roomArray.size() - 1)]->getFreeSpace(returnX, returnY, x, y, true);

			if (teleportMe == true)//just doesnt teleport you if it picks a full room
			{
				roomArray[roomNumber]->actorArray[actorNumber]->x = returnX;
				roomArray[roomNumber]->actorArray[actorNumber]->y = returnY;
				roomArray[roomNumber]->actorArray[actorNumber]->setDrawXY();
			}
		}
	}

	else if (inventory[menuState]->subType == cucumberPlantType)//cucumber
		cucumberMovesActor(roomNumber, actorNumber, roomArray);

	else if (inventory[menuState]->subType == cherryPlantType)//cherry
	{
		//*BOOM*
		for (int eX = roomArray[roomNumber]->actorArray[actorNumber]->x - 1; eX <= roomArray[roomNumber]->actorArray[actorNumber]->x + 1; ++eX)
		{
			for (int eY = roomArray[roomNumber]->actorArray[actorNumber]->y - 1; eY <= roomArray[roomNumber]->actorArray[actorNumber]->y + 1; ++eY)
			{
				if (eX == roomArray[roomNumber]->actorArray[actorNumber]->x &&
					eY == roomArray[roomNumber]->actorArray[actorNumber]->y)
					continue;

				if (eX == x &&
					eY == y)
				{
					takeTrueDamage(ceil((double)health / 2));
					continue;
				}

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == eX &&
							roomArray[i]->tileArray[j]->y == eY)
						{
							roomArray[i]->tileArray[j]->rumble = 5;
							goto cropAffectsActorCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->x == eX &&
							roomArray[i]->roomTileArray[j]->y == eY)
						{
							roomArray[i]->roomTileArray[j]->rumble = 5;
							goto cropAffectsActorCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == eX &&
							roomArray[i]->hallwayArray[j]->y == eY)
						{
							roomArray[i]->hallwayArray[j]->rumble = 5;
							goto cropAffectsActorCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == eX &&
							roomArray[i]->growingPlantArray[j]->y == eY)
						{
							roomArray[i]->growingPlantArray[j]->exists = false;
							goto cropAffectsActorCherryContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == eX &&
							roomArray[i]->actorArray[j]->y == eY)
						{
							roomArray[i]->actorArray[j]->status.exists = false;
							goto cropAffectsActorCherryContinue;
						}
					}
				}
			cropAffectsActorCherryContinue:;
			}
		}

		roomArray[roomNumber]->actorArray[actorNumber]->status.exists = false;//explodes enemy too
	}

	else if (inventory[menuState]->subType == bananaPlantType)//banana
	{
		if (roomArray.size() > 0)
		{
			int randomRoom = getRandomInt(0, roomArray.size() - 1);
			int randomX;
			int randomY;

			if (roomArray[randomRoom]->getFreeSpace(randomX, randomY, x, y, true) == true)
				roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}
	}

	else if (inventory[menuState]->subType == applePlantType)//apple
		roomArray[roomNumber]->actorArray[actorNumber]->status.addStatusEffect(asleepStatusEffect, 10);

	else if (inventory[menuState]->subType == orangePlantType)//orange
	{
		std::vector<SDL_Rect> freeSpaceArray;
		int summonX;
		int summonY;
		std::vector<std::pair<int, int>> summonEnemyArray;

		for (int eX = roomArray[roomNumber]->actorArray[actorNumber]->x - 1; eX <= roomArray[roomNumber]->actorArray[actorNumber]->x + 1; ++eX)
		{
			for (int eY = roomArray[roomNumber]->actorArray[actorNumber]->y - 1; eY <= roomArray[roomNumber]->actorArray[actorNumber]->y + 1; ++eY)
			{
				if (eX == roomArray[roomNumber]->actorArray[actorNumber]->x &&
					eY == roomArray[roomNumber]->actorArray[actorNumber]->y)
					continue;

				for (int i = 0; i < roomArray.size(); ++i)
				{
					for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
					{
						if (roomArray[i]->tileArray[j]->x == eX &&
							roomArray[i]->tileArray[j]->y == eY)
						{
							goto cropAffectsActorOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
					{
						if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap &&
							roomArray[i]->roomTileArray[j]->x == eX &&
							roomArray[i]->roomTileArray[j]->y == eY)
						{
							goto cropAffectsActorOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
					{
						if (roomArray[i]->hallwayArray[j]->x == eX &&
							roomArray[i]->hallwayArray[j]->y == eY)
						{
							goto cropAffectsActorOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
					{
						if (roomArray[i]->growingPlantArray[j]->exists == true &&
							roomArray[i]->growingPlantArray[j]->x == eX &&
							roomArray[i]->growingPlantArray[j]->y == eY)
						{
							goto cropAffectsActorOrangeContinue;
						}
					}

					for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
					{
						if (roomArray[i]->actorArray[j]->status.exists == true &&
							roomArray[i]->actorArray[j]->x == eX &&
							roomArray[i]->actorArray[j]->y == eY)
						{
							goto cropAffectsActorOrangeContinue;
						}
					}
				}

				freeSpaceArray.push_back(SDL_Rect { eX, eY, 0, 0 });

			cropAffectsActorOrangeContinue:;
			}
		}

		if (freeSpaceArray.size() > 0)//if theres space next to you
		{
			int randomFreeSpace = getRandomInt(0, freeSpaceArray.size() - 1);
			summonX = freeSpaceArray[randomFreeSpace].x;
			summonY = freeSpaceArray[randomFreeSpace].y;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true)
						summonEnemyArray.push_back(std::make_pair(i, j));
				}
			}


			if (summonEnemyArray.size() > 0)//if there are enemies to summon
			{
				int randomSummon = getRandomInt(0, summonEnemyArray.size() - 1);
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->x = summonX;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->y = summonY;
				roomArray[summonEnemyArray[randomSummon].first]->actorArray[summonEnemyArray[randomSummon].second]->setDrawXY();
			}
		}
	}
	
	else if (inventory[menuState]->subType == eggplantPlantType)//eggplant
		roomArray[roomNumber]->actorArray[actorNumber]->status.addStatusEffect(poisonedStatusEffect, 10);

	else if (inventory[menuState]->subType == wheatPlantType)//wheat
	{
		if (roomArray[roomNumber]->actorArray[actorNumber]->status.hasNegativeStatusEffect() == true)
			roomArray[roomNumber]->actorArray[actorNumber]->status.friendly = true;
		roomArray[roomNumber]->actorArray[actorNumber]->status.removeAllStatusEffects();
	}

	else if (inventory[menuState]->subType == oatPlantType)//oat
	{
		if (roomArray[roomNumber]->actorArray[actorNumber]->health < roomArray[roomNumber]->actorArray[actorNumber]->maxHealth)
			roomArray[roomNumber]->actorArray[actorNumber]->status.friendly = true;
		roomArray[roomNumber]->actorArray[actorNumber]->addHealth(roomArray[roomNumber]->actorArray[actorNumber]->maxHealth);
	}

	else if (inventory[menuState]->subType == redPepperPlantType)//red pepper
		roomArray[roomNumber]->actorArray[actorNumber]->status.exists = false;

	else if (inventory[menuState]->subType == yellowPepperPlantType)//yellow pepper
		roomArray[roomNumber]->actorArray[actorNumber]->status.addStatusEffect(stunnedStatusEffect, 10);

	else if (inventory[menuState]->subType == lemonPlantType)//lemon
	{
		if (roomArray[roomNumber]->actorArray[actorNumber]->evolveType != evolveTypeNone)
			roomArray[roomNumber]->actorArray[actorNumber]->powerUp(memoryTextArray);
	}

	else if (inventory[menuState]->subType == cornPlantType)//corn
		roomArray[roomNumber]->actorArray[actorNumber]->status.addStatusEffect(shrunkenStatusEffect, 10);

	else if (inventory[menuState]->subType == grapePlantType)//grape
		roomArray[roomNumber]->actorArray[actorNumber]->status.addStatusEffect(confuseStatusEffect, 10);
}

void Player::floorInteract(std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (gameState == gameStateFloorMenu)
		justMoved = true;

	if (justMoved == true &&
		playerTurn >= true)
	{
		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
			{
				if (x == roomArray[i]->itemArray[j]->x &&
					y == roomArray[i]->itemArray[j]->y)
				{
					if (roomArray[i]->itemArray[j]->itemType != equipItemType)
					{
						objectInteract(i, j, roomArray, memoryTextArray);
						goto floorInteractBreak;
					}

					else
					{
						equipInteract(i, j, roomArray, memoryTextArray);
						goto floorInteractBreak;
					}
				}
			}

			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (x == roomArray[i]->roomTileArray[j]->x &&
					y == roomArray[i]->roomTileArray[j]->y &&
					roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap)
				{
					trapInteract(i, j, roomArray, memoryTextArray);

					if (getRandomInt(1, 100) <= 25)
						roomArray[i]->roomTileArray.erase(roomArray[i]->roomTileArray.begin() + j);

					goto floorInteractBreak;
				}
			}
		}

	floorInteractBreak:;
		justMoved = false;
	}
}

void Player::objectInteract(int& roomArrayNumber, int& itemArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (gameState == gameStateIngame)//when first stepping on it
	{
		standingOnObject = true;

		if (inventory.size() < 5 &&
			preciseStep == false &&
			status.fleeing == false)
		{
			standingOnObject = false;
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Picked up " + roomArray[roomArrayNumber]->itemArray[itemArrayNumber]->name + roomArray[roomArrayNumber]->itemArray[itemArrayNumber]->addName + ".")));
			inventory.push_back(std::move(roomArray[roomArrayNumber]->itemArray[itemArrayNumber]));
			roomArray[roomArrayNumber]->itemArray.erase(roomArray[roomArrayNumber]->itemArray.begin() + itemArrayNumber);
		}
	}
}

void Player::equipInteract(int& roomArrayNumber, int& itemArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (gameState == gameStateIngame)//when first stepping on it menu opens floorMenu
	{
		if (preciseStep == false &&
			status.fleeing == false)
		{
			gameState = gameStateFloorMenu;
			menuState = floorMenuEquip;
		}

		standingOnObject = true;
	}

	if (gameState == gameStateFloorMenu)
	{
		if (upKey == true)
		{
			menuState--;

			if (menuState < floorMenuEquip)
				menuState = floorMenuIgnore;
		}

		else if (downKey == true)
		{
			menuState++;

			if (menuState > floorMenuIgnore)
				menuState = floorMenuEquip;
		}

		if (attackKey == true)
		{
			if (menuState == floorMenuEquip)
			{
				std::unique_ptr<Item> tempEquipHolder;
				standingOnObject = false;

				if (equippedItem.get() != nullptr)//drop current cropEquip in the pickups place
				{
					standingOnObject = true;
					equippedItem->x = roomArray[roomArrayNumber]->itemArray[itemArrayNumber]->x;
					equippedItem->y = roomArray[roomArrayNumber]->itemArray[itemArrayNumber]->y;
					tempEquipHolder = std::move(equippedItem);
					equippedItem.reset();
				}

				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Equipped " + roomArray[roomArrayNumber]->itemArray[itemArrayNumber]->name + ".")));
				equippedItem = std::move(roomArray[roomArrayNumber]->itemArray[itemArrayNumber]);
				roomArray[roomArrayNumber]->itemArray.erase(roomArray[roomArrayNumber]->itemArray.begin() + itemArrayNumber);

				if (tempEquipHolder.get() != nullptr)
				{
					roomArray[roomArrayNumber]->itemArray.push_back(std::move(tempEquipHolder));
					tempEquipHolder.reset();
				}

				endTurn();
				return;
			}

			else
			{
				gameState = gameStateIngame;
				menuState = 0;
			}
		}

		else if (facingKey == true)
		{
			gameState = gameStateIngame;
			menuState = 0;
		}
	}
}

void Player::trapInteract(int& roomArrayNumber, int& roomTileArrayNumber, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	rumble = 5;
	memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Oh no!")));
	roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->discovered = true;

	if (getRandomInt(1, 100) <= 10)
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...I avoided something bad.")));

	else if (equippedItem.get() != nullptr &&
		equippedItem->slottedCrop.get() != nullptr &&
		equippedItem->slottedCrop->subType == bananaPlantType &&
		equippedItem->strength > 0)
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...I avoided something bad.")));
		equippedItem->powerSave(memoryTextArray);
	}

	else
	{
		if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == gasTrapType)
		{
			if(roomArrayNumber == 0)
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("A gas leak!")));
			else
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("A strange mushroom!")));
			int randomChance = getRandomInt(0, maxStatusEffectListSize - 1);
			statusEffectList randomStatusEffect = (statusEffectList)randomChance;

			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Strange gas engulfs me...")));
			status.addStatusEffect(randomStatusEffect, 5);

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomArrayNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
						roomArray[i]->actorArray[j]->status.addStatusEffect(randomStatusEffect, 5);
				}
			}
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == dropTrapType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I dropped something!")));

			if (inventory.size() > 0)
			{
				int droppedItem = getRandomInt(0, inventory.size() - 1);
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Lost " + inventory[droppedItem]->name + inventory[droppedItem]->addName + "...")));
				inventory.erase(inventory.begin() + droppedItem);
			}

			else
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...But I had nothing.")));
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == trapTrapType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I feel unlucky...")));

			int randomRoom = getRandomInt(0, roomArray.size() - 1);
			int randomX;
			int randomY;

			if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
				roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == forgetTrapType)
			forgetHerbPower(roomArray, memoryTextArray);

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == sleepTrapType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I fell asleep...")));
			status.addStatusEffect(asleepStatusEffect, 30);
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == hungerTrapType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("I'm hungry...")));
			status.fastHunger = true;
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == wakeTrapType)
		{
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Nearby animals woke up...")));

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomArrayNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
						roomArray[i]->actorArray[j]->status.asleep = false;
				}
			}
		}
	}
}

void Player::endTurn()
{
	timeDownStatus();

	if (status.hastened >= true &&
		playerTurn == true)
		playerTurn = waitingForSecondTurn;
	else if (playerTurn == secondTurn)
		playerTurn = afterSecondTurn;
	else
		playerTurn = false;
	
	statusChecked = false;
	gameState = gameStateIngame;
}

void Player::timeDownStatus()
{
	status.timeDownStatusEffects();

	if (status.shrunken == false)
		maxHealth = baseMaxHealth;
}

void Player::draw(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	int baseDrawX = x * spriteSheetRect.w;
	int baseDrawY = y * spriteSheetRect.h;
	int lastXDraw = lastX * spriteSheetRect.w;
	int lastYDraw = lastY * spriteSheetRect.h;

	if (drawX < baseDrawX)
	{
		if (drawX < lastXDraw)
			drawX = lastXDraw;

		drawX += (baseDrawX - lastXDraw) / 8;

		if (drawX > baseDrawX ||
			lastXDraw > baseDrawX)
			drawX = baseDrawX;
	}

	else if (drawX > baseDrawX)
	{
		if (drawX > lastXDraw)
			drawX = lastXDraw;

		drawX -= (lastXDraw - baseDrawX) / 8;

		if (drawX < baseDrawX ||
			lastXDraw < baseDrawX)
			drawX = baseDrawX;
	}

	if (drawY < baseDrawY)
	{
		if (drawY < lastYDraw)
			drawY = lastYDraw;

		drawY += (baseDrawY - lastYDraw) / 8;

		if (drawY > baseDrawY ||
			lastYDraw > baseDrawY)
			drawY = baseDrawY;
	}

	else if (drawY > baseDrawY)
	{
		if (drawY > lastYDraw)
			drawY = lastYDraw;

		drawY -= (lastYDraw - baseDrawY) / 8;

		if (drawY < baseDrawY ||
			lastYDraw < baseDrawY)
			drawY = baseDrawY;
	}

	gameView.x = round(drawX + (double)spriteSheetRect.w / 2 - (double)gameView.w / 2);//center of players drawX/Y
	gameView.y = round(drawY + (double)spriteSheetRect.h / 2 - (double)gameView.h / 2);

	if (status.exists == true)
	{
		gameViewDrawX = round(drawX) - gameView.x;
		gameViewDrawY = round(drawY) - gameView.y;

		if (rumble > 0 &&
			attackAnimating == false)
		{
			int roundRumble = round(rumble);
			drawXRelative = getRandomInt(-roundRumble, roundRumble);
			drawYRelative = getRandomInt(-roundRumble, roundRumble);
		}

		setFacingSprite();

		SDL_Rect drawingRect = spriteSheetRect;

		if (status.shrunken >= true)
		{
			drawingRect.x = 8;
			drawingRect.y = 8;
		}

		txSpriteSheet->renderClip(gameViewDrawX + drawXRelative, gameViewDrawY + drawYRelative, gRenderer, drawingRect);

		rumble -= rumble / 20;//rumble

		if (drawXRelative > 0)//draw relative
			drawXRelative -= 1;
		else if (drawXRelative < 0)
			drawXRelative += 1;

		if (drawYRelative > 0)
			drawYRelative -= 1;
		else if (drawYRelative < 0)
			drawYRelative += 1;

		if (rumble < 1)
			rumble = 0;

		if (abs(drawXRelative) == 0 &&
			abs(drawYRelative) == 0)
			attackAnimating = false;

		if (gameState == gameStateFacing)
			drawFacingArrow(gRenderer, txSpriteSheet);
	}
}

void Player::setGameViewAndDrawXY(SDL_Rect& gameView)
{
	gameView.x = round((x * 8) + (double)spriteSheetRect.w / 2 - (double)gameView.w / 2);//gameview's center is in the center of players drawX/Y
	gameView.y = round((y * 8) + (double)spriteSheetRect.h / 2 - (double)gameView.h / 2);
	setDrawXY();
}

void Player::setFacingSprite()
{
	if (facing == facingUpLeft)
	{
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 0;
	}

	else if (facing == facingUp)
	{
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 0;
	}

	else if (facing == facingUpRight)
	{
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 0;
	}

	else if (facing == facingLeft)
	{
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 8;
	}

	else if (facing == facingRight)
	{
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 8;
	}

	else if (facing == facingDownLeft)
	{
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 16;
	}

	else if (facing == facingDown)
	{
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 16;
	}

	else if (facing == facingDownRight)
	{
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 16;
	}
}

void Player::drawFacingArrow(SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	int x = 0;
	int y = 0;

	if (facing == facingUpLeft)
	{
		x = -8;
		y = -8;
		facingArrowSpriteSheetRect.x = 0;
		facingArrowSpriteSheetRect.y = 24;
	}

	else if (facing == facingUp)
	{
		x = 0;
		y = -8;
		facingArrowSpriteSheetRect.x = 8;
		facingArrowSpriteSheetRect.y = 24;
	}

	else if (facing == facingUpRight)
	{
		x = 8;
		y = -8;
		facingArrowSpriteSheetRect.x = 16;
		facingArrowSpriteSheetRect.y = 24;
	}

	else if (facing == facingLeft)
	{
		x = -8;
		y = 0;
		facingArrowSpriteSheetRect.x = 0;
		facingArrowSpriteSheetRect.y = 32;
	}

	else if (facing == facingRight)
	{
		x = 8;
		y = 0;
		facingArrowSpriteSheetRect.x = 16;
		facingArrowSpriteSheetRect.y = 32;
	}

	else if (facing == facingDownLeft)
	{
		x = -8;
		y = 8;
		facingArrowSpriteSheetRect.x = 0;
		facingArrowSpriteSheetRect.y = 40;
	}

	else if (facing == facingDown)
	{
		x = 0;
		y = 8;
		facingArrowSpriteSheetRect.x = 8;
		facingArrowSpriteSheetRect.y = 40;
	}

	else if (facing == facingDownRight)
	{
		x = 8;
		y = 8;
		facingArrowSpriteSheetRect.x = 16;
		facingArrowSpriteSheetRect.y = 40;
	}

	txSpriteSheet->renderClip(gameViewDrawX + x, gameViewDrawY + y, gRenderer, facingArrowSpriteSheetRect);
}