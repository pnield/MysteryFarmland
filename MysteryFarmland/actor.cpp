#include "actor.h"
#include "room.h"

Actor::Actor(int x, int y, actorTypeList actorType, bool isAsleep)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();

	rolledForActive = false;
	this->actorType = actorType;
	setType();
	canSeeEnemy = false;
	enemyActorRoomArrayNumber = -1;
	enemyActorActorArrayNumber = -1;
	targetRoom = roomUninitialized;
	targetNode = nodeUninitialized;

	if (isAsleep == true)
		status.asleep = 999;
}

Actor::~Actor()
{
	free();
}

void Actor::free()
{
	heldItem.reset();
}

void Actor::setType()
{
	evolveType = evolveTypeNone;
	devolveType = evolveTypeNone;
	baseAttackPower = 1;
	baseDefensePower = 0;
	dodgeChance = 3;

	if (actorType == chickActorType)
	{
		name = "Chick";
		actorStyle = normalActorStyle;
		evolveType = chickenActorType;
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 96;
		setSpawnHealth(4);
	}

	else if (actorType == chickenActorType)
	{
		name = "Chicken";
		actorStyle = normalActorStyle;
		evolveType = roosterActorType;
		devolveType = chickActorType;
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 96;
		setSpawnHealth(8);
		baseAttackPower = 2;
	}

	else if (actorType == roosterActorType)
	{
		name = "Rooster";
		actorStyle = normalActorStyle;
		devolveType = chickenActorType;
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 96;
		setSpawnHealth(8);
		baseAttackPower = 3;
	}

	else if (actorType == squirrelActorType)
	{
		name = "Squirrel";
		actorStyle = huntSeedsActorStyle;
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 104;
		setSpawnHealth(4);
		dodgeChance = 9;
	}

	else if (actorType == snailActorType)
	{
		name = "Snail";
		actorStyle = normalActorStyle;
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 104;
		setSpawnHealth(2);
		status.blocking = 999;
		status.confused = 999;
		status.slowed = 999;
	}

	else if (actorType == mouseActorType)
	{
		name = "Mouse";
		actorStyle = thiefActorStyle;
		evolveType = ratActorType;
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 104;
		setSpawnHealth(5);
		baseDefensePower = 1;
		dodgeChance = 9;
	}

	else if (actorType == ratActorType)
	{
		name = "Rat";
		actorStyle = thiefActorStyle;
		devolveType = mouseActorType;
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 104;
		setSpawnHealth(5);
		baseDefensePower = 2;
		dodgeChance = 9;
	}

	else if (actorType == crowActorType)
	{
		name = "Crow";
		actorStyle = huntCropsActorStyle;
		evolveType = vultureActorType;
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 112;
		setSpawnHealth(6);
		baseAttackPower = 2;
	}

	else if (actorType == vultureActorType)
	{
		name = "Vulture";
		actorStyle = huntCropsActorStyle;
		devolveType = crowActorType;
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 112;
		setSpawnHealth(8);
		baseAttackPower = 3;
	}

	else if (actorType == kittenActorType)
	{
		name = "Kitten";
		actorStyle = enemyWakerActorStyle;
		evolveType = catActorType;
		spriteSheetRect.x = 0;
		spriteSheetRect.y = 120;
		setSpawnHealth(5);
		baseDefensePower = 1;
		dodgeChance = 9;
		status.fleeing = 999;
	}

	else if (actorType == catActorType)
	{
		name = "Cat";
		actorStyle = enemyWakerActorStyle;
		devolveType = kittenActorType;
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 120;
		setSpawnHealth(5);
		baseAttackPower = 2;
		baseDefensePower = 2;
		dodgeChance = 9;
		status.fleeing = 999;
	}

	else if (actorType == hyenaActorType)
	{
		name = "Hyena";
		actorStyle = enemyWakerActorStyle;
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 120;
		setSpawnHealth(6);
		baseAttackPower = 1;
		baseDefensePower = 4;
		dodgeChance = 9;
		status.fleeing = 999;
	}

	else if (actorType == cowActorType)
	{
		name = "Cow";
		actorStyle = normalActorStyle;
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 96;
		setSpawnHealth(12);
		baseAttackPower = 2;
		dodgeChance = 0;
		status.confused = 999;
		status.slowed = 999;
	}

	else if (actorType == bullActorType)
	{
		name = "Bull";
		actorStyle = normalActorStyle;
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 96;
		setSpawnHealth(12);
		baseAttackPower = 2;
		dodgeChance = 0;
	}
}

void Actor::setSpawnHealth(int number)
{
	baseMaxHealth = number;
	maxHealth = baseMaxHealth;
	health = maxHealth;
}

bool Actor::move(int moveX, int moveY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int tileStatus = isTileFree(x + moveX, y + moveY, roomArray);

	if (x + moveX == player->x &&
		y + moveY == player->y)
	{
		tileStatus = false;
	}

	if (tileStatus != false)
	{
		if (tileStatus == true)
		{
			setLastXYOnMove(moveX, moveY);
			x += moveX;
			y += moveY;
			return true;
		}

		else if (tileStatus == growingPlantBlocked &&
			status.friendly == false &&
			status.fleeing == false)
		{
			attackGrowingPlant(x + moveX, y + moveY, roomArray, memoryTextArray);
			return true;
		}
	}

	return false;
}

bool Actor::moveDiagonal(int moveX, int moveY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (player->x != x + moveX ||
		player->y != y + moveY)
	{
		int diagTileFree = isTileFreeDiagonal(x + moveX, y + moveY, x + moveX, y, x, y + moveY, roomArray, true);

		if (diagTileFree == false)
			return false;
		else
		{
			if (diagTileFree == true)
			{
				setLastXYOnMove(moveX, moveY);
				x += moveX;
				y += moveY;
				return true;
			}

			else if (diagTileFree == growingPlantBlocked &&
				status.friendly == false &&
				status.fleeing == false)
			{
				attackGrowingPlant(x + moveX, y + moveY, roomArray, memoryTextArray);
				return true;
			}
		}
	}

	return false;
}

bool Actor::moveAroundACorner(int moveX, int moveY, int cornerBlockedTileX, int cornerBlockedTileY, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int tileStatus = isTileACorner(x + moveX, y + moveY, x + cornerBlockedTileX, y + cornerBlockedTileY, roomArray);

	if (x + moveX == player->x &&
		y + moveY == player->y)
	{
		tileStatus = false;
	}

	if (tileStatus != false)
	{
		if (tileStatus == true)
		{
			setLastXYOnMove(moveX, moveY);
			x += moveX;
			y += moveY;
		}

		else if (tileStatus == growingPlantBlocked &&
			status.friendly == false &&
			status.fleeing == false)
			attackGrowingPlant(x + moveX, y + moveY, roomArray, memoryTextArray);

		return true;
	}

	return false;
}

void Actor::drunkAct(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int targetX = 0;
	int targetY = 0;
	facing = getRandomInt(0, maxFacingListSize - 1);
	targetFacing(targetX, targetY, false);

	if (status.fleeing == false)
	{
		if (player->x == x + targetX &&
			player->y == y + targetY &&
			isPlayerInRange(true, player, roomArray) == true)//if theres a player or an actor in faced direction, attack them
		{
			attackPlayer(player, roomArray, memoryTextArray);
			return;
		}

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->x == x + targetX &&
					roomArray[i]->growingPlantArray[j]->y == y + targetY &&
					isGrowingPlantInRange(true, roomArray[i]->growingPlantArray[j], roomArray) == true)
				{
					attackGrowingPlant(x + targetX, y + targetY, roomArray, memoryTextArray);
					return;
				}
			}

			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				enemyActorRoomArrayNumber = i;
				enemyActorActorArrayNumber = j;

				if (roomArray[i]->actorArray[j]->x == x + targetX &&
					roomArray[i]->actorArray[j]->y == y + targetY &&
					isEnemyActorInRange(true, false, roomArray[i]->actorArray[j], roomArray) == true)
				{
					attackActor(roomArray[i]->actorArray[j], player, roomArray, memoryTextArray);
					return;
				}
			}
		}
	}

	if (targetX == 0 ||
		targetY == 0)
	{
		if (move(targetX, targetY, player, roomArray, memoryTextArray) == true)//else if theres no one there, try walking to that space
			;
	}

	else
	{
		if (moveDiagonal(targetX, targetY, player, roomArray, memoryTextArray) == true)
			;
	}
}

void Actor::attackPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	setFacing(player->x, player->y, true);

	int damageDealt;
	int rumbleDealt;
	attackingDamage(damageDealt, rumbleDealt);

	bool hitLanded = player->takeDefendableDamage(damageDealt);

	if (hitLanded == true &&
		player->status.blocking >= true)
	{
		signed int moveX = 0;
		signed int moveY = 0;
		signed int targetMoveX = 0;
		signed int targetMoveY = 0;

		blockPushback(player->x, player->y, moveX, moveY, targetMoveX, targetMoveY, roomArray);

		if (moveX != 0 ||
			moveY != 0)
		{
			setLastXYOnMove(moveX, moveY);
			x += moveX;
			y += moveY;
		}

		if (targetMoveX != 0 ||
			targetMoveY != 0)
		{
			player->setLastXYOnMove(targetMoveX, targetMoveY);
			player->x += targetMoveX;
			player->y += targetMoveY;
			player->onMove(false);
		}
	}

	if (player->status.exists == false)//purposely not if visible == true so player knows what killed them even if they're blind or something
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("[" + name + "] beat me!")));
}

void Actor::attackActor(std::unique_ptr<Actor>& targetActor, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	setFacing(targetActor->x, targetActor->y, true);

	int damageDealt;
	int rumbleDealt;
	attackingDamage(damageDealt, rumbleDealt);

	bool hitLanded = targetActor->takeDefendableDamage(damageDealt);

	if (hitLanded == true &&
		targetActor->status.blocking >= true)
	{
		signed int moveX = 0;
		signed int moveY = 0;
		signed int targetMoveX = 0;
		signed int targetMoveY = 0;

		blockPushback(targetActor->x, targetActor->y, moveX, moveY, targetMoveX, targetMoveY, roomArray);

		if (moveX != 0 ||
			moveY != 0)
		{
			if (x + moveX != player->x ||
				y + moveY != player->y)
			{
				setLastXYOnMove(moveX, moveY);
				x += moveX;
				y += moveY;
			}
		}

		if (targetMoveX != 0 ||
			targetMoveY != 0)
		{
			if (x + targetMoveX != player->x ||
				y + targetMoveY != player->y)
			{
				targetActor->setLastXYOnMove(targetMoveX, targetMoveY);
				targetActor->x += targetMoveX;
				targetActor->y += targetMoveY;
			}
		}
	}

	if (targetActor->status.exists == false &&
		visible == true)
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("[" + name + "] calmed [" + targetActor->name + "]!")));
}

void Actor::attackGrowingPlant(int attackX, int attackY, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (roomArray[i]->growingPlantArray[j]->exists == true &&
				roomArray[i]->growingPlantArray[j]->x == attackX &&
				roomArray[i]->growingPlantArray[j]->y == attackY)
			{
				//*chomp!*
				setFacing(attackX, attackY, true);
				roomArray[i]->growingPlantArray[j]->takeDamage(1, 5);

				if (roomArray[i]->growingPlantArray[j]->exists == false)
				{
					if (visible == true)
						memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(name + " plucked plant.")));

					if (actorStyle == huntCropsActorStyle)
					{
						heldItem.reset();
						heldItem = std::make_unique<Item>((itemTypeList)seedItemType, roomArray[i]->growingPlantArray[j]->grownCrop->subType);
					}
				}
			}
		}
	}
}

void Actor::attackingDamage(int& damageDealt, int& rumbleDealt)
{
	damageDealt = generateAttackDamage(baseAttackPower);
	rumbleDealt = 5;

	if (status.shrunken > 0)
	{
		damageDealt = floor((double)damageDealt / 2);
		rumbleDealt = 2;
	}
}

bool Actor::ifInRangeDiagonalBlocked(int targetX, int targetY, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (targetX == x - 1 &&
		targetY == y - 1)//if target is up left of me
	{
		if (isTileFreeDiagonal(x - 1, y - 1, x - 1, y, x, y - 1, roomArray, false) == false)//if theres a wall to the side of the diagonal attack, attack doesnt work
			return false;
	}

	else if (targetX == x - 1 &&
		targetY == y + 1)
	{
		if (isTileFreeDiagonal(x - 1, y + 1, x - 1, y, x, y + 1, roomArray, false) == false)
			return false;
	}

	else if (targetX == x + 1 &&
		targetY == y + 1)
	{
		if (isTileFreeDiagonal(x + 1, y + 1, x + 1, y, x, y + 1, roomArray, false) == false)
			return false;
	}

	else if (targetX == x + 1 &&
		targetY == y - 1)
	{
		if (isTileFreeDiagonal(x + 1, y - 1, x + 1, y, x, y - 1, roomArray, false) == false)
			return false;
	}

	return true;
}

void Actor::powerUp(std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	rumble = 5;
	status.removeNegativeStatusEffects();

	if (evolveType != evolveTypeNone)
	{
		//*poof!*
		if (visible == true)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(name + " powered up...")));
		actorType = (actorTypeList)evolveType;//evolves into evolveType
		setType();
		if (visible == true)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("...and evolved into " + name + "!")));
	}

	else
	{
		//*shine sound effect*
		if (visible == true)
			memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(name + " recovered!")));
		health = maxHealth;//health is regenerated
	}
}

void Actor::dropItem(std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (heldItem.get() != nullptr)
	{
		int dropX = x;
		int dropY = y;
		int returnRoom = -1;

		if (getFreeDroppingSpace(dropX, dropY, returnRoom, roomArray) == true)
		{
			heldItem->x = dropX;
			heldItem->y = dropY;
			heldItem->setDrawXY();
			roomArray[returnRoom]->itemArray.push_back(std::move(heldItem));
			heldItem.reset();
		}
	}
}

void Actor::setTargetToPlayersNode(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			for (int j = 0; j < roomArray[i]->nodeArray.size(); ++j)
			{
				if (player->x == roomArray[i]->nodeArray[j]->x &&
					player->y == roomArray[i]->nodeArray[j]->y)//make function and change break into return. only do this is player is in my room
				{
					targetRoom = i;
					targetNode = j;
					break;
				}
			}
		}
	}
}

bool Actor::isPlayerInRange(bool attack, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (player->status.exists == true &&
		isInThisDistanceOfMe(player->x, player->y, 1) == true &&
		player->status.shrunken == false)//if in 1 tile range of player
	{
		if (attack == false)
			return true;

		else if (status.fleeing == false)
			return ifInRangeDiagonalBlocked(player->x, player->y, roomArray);//check if attack isnt blocked by a wall
	}

	return false;
}

bool Actor::isEnemyActorInRange(bool attack, bool honorable, std::unique_ptr<Actor>& targetActor, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->status.exists == true &&
		roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->status.shrunken == false &&
		isInThisDistanceOfMe(roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->x, roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->y, 1) == true)
	{
		if (honorable == true)
		{
			if (roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->status.friendly == status.friendly ||
				roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->status.asleep >= 999)
				return false;
		}

		if (attack == false)
			return true;

		else if (status.fleeing == false)
			return ifInRangeDiagonalBlocked(roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->x, roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber]->y, roomArray);//check if attack isnt blocked by a wall
	}

	return false;
}

bool Actor::isGrowingPlantInRange(bool attack, std::unique_ptr<GrowingPlant>& targetGrowingPlant, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (targetGrowingPlant->exists == true &&
		isInThisDistanceOfMe(targetGrowingPlant->x, targetGrowingPlant->y, 1) == true)
	{
		if (attack == false)
			return true;

		else if (status.fleeing == false)
			return ifInRangeDiagonalBlocked(targetGrowingPlant->x, targetGrowingPlant->y, roomArray);
	}

	return false;
}

void Actor::update(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{	
	canSeeEnemy = false;

	checkStatus();

	if (status.exists == true &&
		status.missThisTurn == false)
	{
		if (status.asleep >= true)
		{
			if (rolledForActive == false &&
				inPlayerRoom(false, player, roomArray) == true)//if player steps into room, not in doorway
			{
				if (status.asleep >= 999 &&
					getRandomInt(1, 100) > 15)//% chance to not wake up
					status.asleep = false;

				rolledForActive = true;
				return;
			}

			if(rolledForActive == false ||
				status.asleep >= 999)
				return;
		}

		if (status.asleep == false &&
			status.stunned == false)
		{
			int movedX = x;
			int movedY = y;

			actorStyleAction(player, roomArray, memoryTextArray);

			if (movedX != x ||
				movedY != y)//checks if you move this turn, if so, you can check for things underneath you
				floorInteract(player, roomArray, memoryTextArray);
		}

		timeDownStatus();
	}
}

void Actor::checkStatus()
{
	if (status.asleep == false)
		status.applyStatus(health, true);
	else
		status.checkDeath(health);//dont apply poison if not visited yet

	if (status.exists == true)
	{
		if (status.shrunken >= true)//shrunken health
		{
			maxHealth = floor(baseMaxHealth / 2);

			if (health > maxHealth)
				health = maxHealth;
		}
	}
}

bool Actor::floorInteract(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			if (roomArray[i]->itemArray[j]->x == x &&
				roomArray[i]->itemArray[j]->y == y &&
				status.friendly == false &&
				status.fleeing == false)
			{
				if (roomArray[i]->itemArray[j]->itemType == seedItemType)
				{
					//*chomp!*
					if (actorStyle == huntSeedsActorStyle)
						heldItem = std::move(roomArray[i]->itemArray[j]);
					powerUp(memoryTextArray);
					roomArray[i]->itemArray.erase(roomArray[i]->itemArray.begin() + j);
					return true;
				}

				else if (actorStyle == thiefActorStyle &&
					heldItem.get() == nullptr)
				{
					heldItem.reset();
					heldItem = std::move(roomArray[i]->itemArray[j]);
					roomArray[i]->itemArray.erase(roomArray[i]->itemArray.begin() + j);
					status.fleeing = 999;
					return true;
				}
			}
		}

		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			if (x == roomArray[i]->roomTileArray[j]->x &&
				y == roomArray[i]->roomTileArray[j]->y &&
				roomArray[i]->roomTileArray[j]->tileType == tileTypeTrap &&
				roomArray[i]->roomTileArray[j]->discovered == true &&
				status.resistStatusEffects == false)
			{
				trapInteract(i, j, player, roomArray, memoryTextArray);

				if (getRandomInt(1, 100) <= 25)
					roomArray[i]->roomTileArray.erase(roomArray[i]->roomTileArray.begin() + j);

				return true;
			}
		}
	}

	return false;
}

void Actor::trapInteract(int& roomArrayNumber, int& roomTileArrayNumber, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (getRandomInt(1, 100) > 10)
	{
		if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == gasTrapType)
		{
			int randomChance = getRandomInt(0, maxStatusEffectListSize - 1);
			statusEffectList randomStatusEffect = (statusEffectList)randomChance;

			for (int i = 0; i < roomArray.size(); ++i)
			{
				for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[roomArrayNumber]->isInRoomAndDoorway(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if actors are within players room
						roomArray[i]->actorArray[j]->status.addStatusEffect(randomStatusEffect, 5);
				}
			}

			if (roomArray[roomArrayNumber]->isInRoomAndDoorway(player->x, player->y) == true)
			{
				memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Strange gas engulfs me...")));
				player->status.addStatusEffect(randomStatusEffect, 5);
			}
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == trapTrapType)
		{
			int randomRoom = getRandomInt(0, roomArray.size() - 1);
			int randomX;
			int randomY;

			if (roomArray[randomRoom]->getFreeSpaceWithoutPlayer(randomX, randomY, false) == true)
				roomArray[randomRoom]->roomTileArray.push_back(std::make_unique<Tile>(randomX, randomY, tileTypeTrap, getRandomInt(0, maxTrapTypeListSize - 1)));
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == forgetTrapType)
			status.addStatusEffect(confuseStatusEffect, 999);

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == sleepTrapType)
			status.addStatusEffect(asleepStatusEffect, 999);

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == hungerTrapType)
		{
			if (status.hastened >= true)
				status.hastened = false;
			else if (status.slowed == false)
				status.addStatusEffect(slowedStatusEffect, 999);
			else
				status.addStatusEffect(stunnedStatusEffect, 999);
		}

		else if (roomArray[roomArrayNumber]->roomTileArray[roomTileArrayNumber]->subType == wakeTrapType)
		{
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

void Actor::actorStyleAction(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (status.confused >= true)
		drunkAct(player, roomArray, memoryTextArray);

	else if (status.fleeing >= true)
	{
		if (actorStyle != enemyWakerActorStyle)
			findRoomToNode(false, player, roomArray, memoryTextArray);

		else
		{
			int myRoom = -1;

			for (int r = 0; r < roomArray.size(); ++r)
			{
				if (roomArray[r]->isInsideRoom(x, y) == true)
				{
					myRoom = r;
					break;
				}
			}

			if (status.shrunken == false &&
				myRoom != -1 &&
				getRandomInt(1, 100) <= 10)
			{
				//make a meow/laugh sound
				rumble = 2;

				if (status.friendly == false)
				{
					for (int i = 0; i < roomArray.size(); ++i)
					{
						for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
						{
							if (roomArray[i]->actorArray[j]->status.exists == true &&
								roomArray[myRoom]->isInsideRoom(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)//if in the same room, remove negative status effects
								roomArray[i]->actorArray[j]->status.removeNegativeStatusEffects();
						}
					}
				}

				else if (status.friendly == true &&
					roomArray[myRoom]->isInsideRoom(player->x, player->y) == true)
					player->status.removeNegativeStatusEffects();
			}

			else
				findRoomToNode(false, player, roomArray, memoryTextArray);
		}
	}

	else
	{
		if (status.friendly == false &&
			actorStyle == huntSeedsActorStyle)
		{
			if (isEnemyActorNearby(true, roomArray) == true)//if an enemy actors in the room or near me
			{
				targetRoom = roomUninitialized;
				targetNode = nodeUninitialized;
				canSeeEnemy = targettingEnemyActor;
			}

			else if (isPlayerInRange(true, player, roomArray) == true)
				attackPlayer(player, roomArray, memoryTextArray);
			else if (heldItem.get() == nullptr)
				huntSeeds(player, roomArray, memoryTextArray);
			else
				findRoomToNode(false, player, roomArray, memoryTextArray);
		}

		else if (status.friendly == false &&
			actorStyle == huntCropsActorStyle)
		{
			if (isEnemyActorNearby(true, roomArray) == true)
			{
				targetRoom = roomUninitialized;
				targetNode = nodeUninitialized;
				canSeeEnemy = targettingEnemyActor;
			}
			else if (isPlayerInRange(true, player, roomArray) == true)
				attackPlayer(player, roomArray, memoryTextArray);
			else if (heldItem.get() == nullptr)
				huntGrowingPlants(player, roomArray, memoryTextArray);
			else
				findRoomToNode(false, player, roomArray, memoryTextArray);
		}

		else if (canSeeEnemy == false)
		{
			if (isEnemyActorNearby(true, roomArray) == true)
			{
				targetRoom = roomUninitialized;
				targetNode = nodeUninitialized;
				canSeeEnemy = targettingEnemyActor;
			}

			else if (status.friendly == false &&
				inPlayerRoom(true, player, roomArray) == true ||
				status.friendly == false &&
				isPlayerInRange(false, player, roomArray) == true)//else if im hostile and the players in the room or near me
			{
				setTargetToPlayersNode(player, roomArray);
				canSeeEnemy = targettingPlayer;
			}
			
			else
			{
				if (status.friendly == false)
					findRoomToNode(true, player, roomArray, memoryTextArray);//if not, move to node. if player isn't inside room but is in doorway, move to that node
				else
					friendlyWander(player, roomArray, memoryTextArray);
			}
		}

		if (canSeeEnemy >= true)
		{
			if (canSeeEnemy == targettingEnemyActor)
			{
				if (isEnemyActorInRange(true, true, roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber], roomArray) == true)
					attackActor(roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber], player, roomArray, memoryTextArray);
				else
					moveToEnemyActor(roomArray[enemyActorRoomArrayNumber]->actorArray[enemyActorActorArrayNumber], player, roomArray, memoryTextArray);//move to enemyActor
			}

			else if (canSeeEnemy == targettingPlayer)
			{
				if (isPlayerInRange(true, player, roomArray) == true)
				{
					if (actorStyle == thiefActorStyle &&
						status.shrunken == false &&
						player->inventory.size() > 0)
						stealFromPlayer(player, roomArray, memoryTextArray);
					else
						attackPlayer(player, roomArray, memoryTextArray);
				}

				else
					moveToPlayer(player, roomArray, memoryTextArray);
			}
		}
	}
}

bool Actor::isEnemyActorNearby(bool enemyInDoorway, std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (roomArray[i]->actorArray[j]->status.friendly != status.friendly)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true &&
					roomArray[i]->actorArray[j]->status.asleep < 999 &&
					roomArray[i]->actorArray[j]->status.shrunken == false &&
					isInThisDistanceOfMe(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y, 1) == true)
				{
					enemyActorRoomArrayNumber = i;
					enemyActorActorArrayNumber = j;
					return true;
				}
			}
		}
	}

	int myRoom = -1;

	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			myRoom = i;
		}
	}

	if (myRoom > -1)
	{
		bool foundEnemy = false;
		int distance = INT_MAX;

		for (int i = 0; i < roomArray.size(); ++i)
		{
			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.friendly != status.friendly)
				{
					if (roomArray[i]->actorArray[j]->status.exists == true &&
						roomArray[i]->actorArray[j]->status.asleep < 999 &&
						roomArray[i]->actorArray[j]->status.shrunken == false &&
						roomArray[myRoom]->isInsideRoom(roomArray[i]->actorArray[j]->x, roomArray[i]->actorArray[j]->y) == true)
					{
						int foundEnemyDistance = abs(roomArray[i]->actorArray[j]->x - x) + abs(roomArray[i]->actorArray[j]->y - y);//the absolute distance between actor and target

						if (foundEnemyDistance < distance)//if this targets abs distance is greater than the remembered abs distance
						{
							distance = foundEnemyDistance;
							foundEnemy = true;
							enemyActorRoomArrayNumber = i;
							enemyActorActorArrayNumber = j;
						}
					}
				}
			}
		}

		return foundEnemy;
	}

	return false;
}

bool Actor::inPlayerRoom(bool playerInDoorway, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (player->status.exists == true &&
		player->status.shrunken == false)
	{
		for (int i = 0; i < roomArray.size(); ++i)
		{
			if (roomArray[i]->isInsideRoom(x, y) == true)
			{
				if (playerInDoorway == true)
				{
					if(roomArray[i]->isInRoomAndDoorway(player->x, player->y) == true)//if actors inside room and can see player in room or doorway
						return true;
				}

				else
				{
					if (roomArray[i]->isInsideRoom(player->x, player->y) == true)
						return true;
				}
			}
		}
	}

	return false;
}

void Actor::actorPathing(bool turnCorners, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (facing == facingUpLeft)
	{
		if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
		else if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownLeft;
		else if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpRight;
		else if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownRight;
	}

	else if (facing == facingDownLeft)
	{
		if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
		else if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownRight;
		else if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpLeft;
		else if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpRight;
	}

	else if (facing == facingDownRight)
	{
		if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
		else if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpRight;
		else if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownLeft;
		else if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpLeft;
	}

	else if (facing == facingUpRight)
	{
		if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
		else if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpLeft;
		else if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownRight;
		else if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownLeft;
	}

	else if (facing == facingUp)
	{
		if (turnCorners == true &&
			moveAroundACorner(1, 0, 1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			;
		else if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpLeft;
		else if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpRight;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
	}

	else if (facing == facingLeft)
	{
		if (turnCorners == true &&
			moveAroundACorner(0, -1, 1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			;
		else if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownLeft;
		else if (moveDiagonal(-1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpLeft;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
	}

	else if (facing == facingDown)
	{
		if (turnCorners == true &&
			moveAroundACorner(-1, 0, -1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			;
		else if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownRight;
		else if (moveDiagonal(-1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownLeft;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingRight;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
	}

	else if (facing == facingRight)
	{
		if (turnCorners == true &&
			moveAroundACorner(0, 1, -1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
		else if (move(1, 0, player, roomArray, memoryTextArray) == true)
			;
		else if (moveDiagonal(1, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUpRight;
		else if (moveDiagonal(1, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDownRight;
		else if (move(0, -1, player, roomArray, memoryTextArray) == true)
			facing = facingUp;
		else if (move(0, 1, player, roomArray, memoryTextArray) == true)
			facing = facingDown;
		else if (move(-1, 0, player, roomArray, memoryTextArray) == true)
			facing = facingLeft;
	}
}

void Actor::stealFromPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	setFacing(player->x, player->y, true);

	int stolenItem = getRandomInt(0, player->inventory.size() - 1);
	heldItem.reset();
	heldItem = std::move(player->inventory[stolenItem]);
	player->inventory.erase(player->inventory.begin() + stolenItem);

	memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>(heldItem->name + heldItem->addName + " stolen!")));

	status.fleeing = 999;
}

void Actor::huntSeeds(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	std::vector<std::pair<int, int>> foundSeedArray;

	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
			{
				if (roomArray[i]->itemArray[j]->itemType == seedItemType)
					foundSeedArray.push_back(std::make_pair(i, j));
			}
		}
	}

	if (foundSeedArray.size() > 0)
	{
		int distance = INT_MAX;//distance to seed, uninitialized
		int facingX;
		int facingY;

		for (int i = 0; i < foundSeedArray.size(); ++i)
		{
			int foundSeedDistance = abs(roomArray[foundSeedArray[i].first]->itemArray[foundSeedArray[i].second]->x - x) + abs(roomArray[foundSeedArray[i].first]->itemArray[foundSeedArray[i].second]->y - y);

			if (foundSeedDistance < distance)
			{
				distance = foundSeedDistance;
				facingX = roomArray[foundSeedArray[i].first]->itemArray[foundSeedArray[i].second]->x;
				facingY = roomArray[foundSeedArray[i].first]->itemArray[foundSeedArray[i].second]->y;
			}
		}

		setFacing(facingX, facingY, false);
		actorPathing(false, player, roomArray, memoryTextArray);
	}

	else
		findRoomToNode(false, player, roomArray, memoryTextArray);
}

void Actor::huntGrowingPlants(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	std::vector<std::pair<int, int>> livingGrowingPlantArray;

	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->exists == true)
					livingGrowingPlantArray.push_back(std::make_pair(i, j));
			}
		}
	}

	if (livingGrowingPlantArray.size() > 0)
	{
		int distance = INT_MAX;//distance to closest growingPlant, uninitialized
		int facingX;
		int facingY;

		for (int i = 0; i < livingGrowingPlantArray.size(); ++i)
		{
			int livingGrowingPlantDistance = abs(roomArray[livingGrowingPlantArray[i].first]->growingPlantArray[livingGrowingPlantArray[i].second]->x - x) + abs(roomArray[livingGrowingPlantArray[i].first]->growingPlantArray[livingGrowingPlantArray[i].second]->y - y);
			
			if (livingGrowingPlantDistance < distance)
			{
				distance = livingGrowingPlantDistance;
				facingX = roomArray[livingGrowingPlantArray[i].first]->growingPlantArray[livingGrowingPlantArray[i].second]->x;
				facingY = roomArray[livingGrowingPlantArray[i].first]->growingPlantArray[livingGrowingPlantArray[i].second]->y;
			}
		}

		setFacing(facingX, facingY, false);
		actorPathing(false, player, roomArray, memoryTextArray);
	}

	else
		findRoomToNode(false, player, roomArray, memoryTextArray);
}

void Actor::moveToEnemyActor(std::unique_ptr<Actor>& targetActor, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	setFacing(targetActor->x, targetActor->y, false);
	actorPathing(false, player, roomArray, memoryTextArray);
}

void Actor::moveToPlayer(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	setFacing(player->x, player->y, false);
	actorPathing(false, player, roomArray, memoryTextArray);
}

void Actor::findRoomToNode(bool followPlayer, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			if (status.fleeing >= true &&
				roomArray[i]->isInRoomAndDoorway(player->x, player->y) == true)
				fleeToNode(i, player, roomArray, memoryTextArray);
			else
				moveToNode(i, followPlayer, player, roomArray, memoryTextArray);
			return;
		}
	}

	targetRoom = roomUninitialized;
	targetNode = nodeUninitialized;//else, reset target node and do the dumb pathing
	actorPathing(true, player, roomArray, memoryTextArray);
}

void Actor::moveToNode(int& roomNumber, bool followPlayer, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if (followPlayer == true)
	{
		for (int i = 0; i < roomArray[roomNumber]->nodeArray.size(); ++i)//if in the middle of your pathing player goes to a doorway node and youre told to follow him, move to that node
		{
			if (player->x == roomArray[roomNumber]->nodeArray[i]->x &&
				player->y == roomArray[roomNumber]->nodeArray[i]->y)
			{
				targetRoom = roomNumber;
				targetNode = i;
				break;
			}
		}
	}

	if (targetNode == nodeUninitialized)
	{
		int absX = -1;//remembered absolute distance between actor and node
		int absY = -1;

		for (int i = 0; i < roomArray[roomNumber]->nodeArray.size(); ++i)
		{
			int calculationX = abs(roomArray[roomNumber]->nodeArray[i]->x - x);//the absolute distance between actor and this node
			int calculationY = abs(roomArray[roomNumber]->nodeArray[i]->y - y);

			if (calculationX > absX &&
				calculationY > absY)//if this nodes abs distance is greater than the remembered abs distance
			{
				targetRoom = roomNumber;
				targetNode = i;//remember the node
				absX = calculationX;//remember the distance
				absY = calculationY;
			}
		}
	}

	if (targetRoom != roomUninitialized &&
		targetNode != nodeUninitialized &&
		roomArray[targetRoom]->isInsideRoom(x, y) == true)
	{
		setFacing(roomArray[roomNumber]->nodeArray[targetNode]->x, roomArray[roomNumber]->nodeArray[targetNode]->y, false);//face towards targetNode

		if (x == roomArray[roomNumber]->nodeArray[targetNode]->x &&
			y == roomArray[roomNumber]->nodeArray[targetNode]->y)//if you reach the node, node is uninitialized again
		{
			targetRoom = roomUninitialized;
			targetNode = nodeUninitialized;
		}
	}

	actorPathing(false, player, roomArray, memoryTextArray);
}

void Actor::fleeToNode(int& roomNumber, std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	targetRoom = roomUninitialized;
	targetNode = nodeUninitialized;

	if (targetNode == nodeUninitialized)
	{
		int absDistance = INT_MAX;

		for (int i = 0; i < roomArray[roomNumber]->nodeArray.size(); ++i)
		{
			int myDistance = abs(roomArray[roomNumber]->nodeArray[i]->x - x) + abs(roomArray[roomNumber]->nodeArray[i]->y - y);
			int playersDistance = abs(roomArray[roomNumber]->nodeArray[i]->x - player->x) + abs(roomArray[roomNumber]->nodeArray[i]->y - player->y);

			if (myDistance <= playersDistance &&
				myDistance <= absDistance)//move to closest node that the player is further away from
			{
				targetRoom = roomNumber;
				targetNode = i;//remember the node
				absDistance = myDistance;//remember the distance
			}
		}
	}

	if (targetRoom != roomUninitialized &&
		targetNode != nodeUninitialized &&
		roomArray[targetRoom]->isInsideRoom(x, y) == true)
	{
		setFacing(roomArray[roomNumber]->nodeArray[targetNode]->x, roomArray[roomNumber]->nodeArray[targetNode]->y, false);//face towards targetNode

		if (x == roomArray[roomNumber]->nodeArray[targetNode]->x &&
			y == roomArray[roomNumber]->nodeArray[targetNode]->y)//if you reach the node, node is uninitialized again
		{
			targetRoom = roomUninitialized;
			targetNode = nodeUninitialized;
		}
	}

	actorPathing(false, player, roomArray, memoryTextArray);
}

void Actor::friendlyWander(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Room>>& roomArray, std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	int targetX = 0;
	int targetY = 0;
	facing = getRandomInt(0, maxFacingListSize - 1);
	targetFacing(targetX, targetY, false);

	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (roomArray[i]->isInsideRoom(x, y) == true)
		{
			if (roomArray[i]->isInsideRoom(x + targetX, y + targetY) == true)
			{
				if (targetX == 0 ||
					targetY == 0)
				{
					if (move(targetX, targetY, player, roomArray, memoryTextArray) == true)
						;
				}

				else
				{
					if (moveDiagonal(targetX, targetY, player, roomArray, memoryTextArray) == true)
						;
				}
			}

			return;
		}
	}

	findRoomToNode(false, player, roomArray, memoryTextArray);
}

void Actor::timeDownStatus()
{
	if (status.missThisTurn == false)
	{
		status.timeDownStatusEffects();

		if (status.shrunken == false)
			maxHealth = baseMaxHealth;
	}
}

void Actor::draw(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	int baseDrawX = x * spriteSheetRect.w;
	int baseDrawY = y * spriteSheetRect.h;

	if (baseDrawX + spriteSheetRect.w + 8 < gameView.x ||
		baseDrawX - 8 > gameView.x + gameView.w ||
		baseDrawY + spriteSheetRect.h + 8 < gameView.y ||
		baseDrawY - 8 > gameView.y + gameView.h)
		visible = false;

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

	gameViewDrawX = drawX - gameView.x;
	gameViewDrawY = drawY - gameView.y;

	if (rumble > 0)
	{
		int roundRumble = round(rumble);
		drawXRelative = getRandomInt(-roundRumble, roundRumble);
		drawYRelative = getRandomInt(-roundRumble, roundRumble);
	}

	SDL_Rect drawingRect = spriteSheetRect;

	if (status.shrunken >= true)
	{
		drawingRect.x = 0;
		drawingRect.y = 96;
	}

	if (visible == true)
	{
		if (facing == facingDown ||
			facing == facingUpRight ||
			facing == facingRight ||
			facing == facingDownRight)
			txSpriteSheet->renderClip(gameViewDrawX + drawXRelative, gameViewDrawY + drawYRelative, gRenderer, drawingRect);
		else
			txSpriteSheet->renderClipHorizontalFlip(gameViewDrawX + drawXRelative, gameViewDrawY + drawYRelative, gRenderer, drawingRect);
	}

	rumble -= rumble / 20;

	if (drawXRelative > 0)
		drawXRelative -= 1;
	else if (drawXRelative < 0)
		drawXRelative += 1;

	if (drawYRelative > 0)
		drawYRelative -= 1;
	else if (drawYRelative < 0)
		drawYRelative += 1;

	if (rumble < 1)
		rumble = 0;
}