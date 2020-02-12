#include "baseLifeform.h"
#include "room.h"

BaseLifeform::BaseLifeform()
{
	gameViewDrawX = 0;
	gameViewDrawY = 0;
	facing = facingDown;
	baseAttackPower = 0;
	baseDefensePower = 0;
	dodgeChance = 3;
	status.initializeStatusEffects();
}

int BaseLifeform::generateAttackDamage(double attackPower)
{
	double variance = getRandomInt(-3, 3);
	variance = variance / 10;
	return round(attackPower + variance);
}

bool BaseLifeform::takeDefendableDamage(int& damageTaken)
{
	if (getRandomInt(1, 100) >= dodgeChance)
	{
		damageTaken -= floor(baseDefensePower);

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

		takeTrueDamage(damageTaken);

		return true;
	}

	else
	{
		//*whoosh!*
		return false;
	}
}

void BaseLifeform::takeTrueDamage(int damageTaken)
{
	health -= damageTaken;
	rumble = 3;
	status.asleep = false;
	status.checkDeath(health);
}

void BaseLifeform::addHealth(int addAmount)
{
	//*sparkle sound effect*
	health += addAmount;

	if (health > maxHealth)
		health = maxHealth;
}

void BaseLifeform::setFacing(int facingX, int facingY, bool animateAttack)
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

void BaseLifeform::targetFacing(int& targetX, int& targetY, bool animateAttack)
{
	if (facing == facingUpLeft)
	{
		targetX = -1;
		targetY = -1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingUpRight)
	{
		targetX = 1;
		targetY = -1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingDownLeft)
	{
		targetX = -1;
		targetY = 1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingDownRight)
	{
		targetX = 1;
		targetY = 1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingUp)
	{
		targetX = 0;
		targetY = -1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingLeft)
	{
		targetX = -1;
		targetY = 0;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingRight)
	{
		targetX = 1;
		targetY = 0;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}

	else if (facing == facingDown)
	{
		targetX = 0;
		targetY = 1;

		if (animateAttack == true)
			animateAttackMove(targetX, targetY);
	}
}

void BaseLifeform::animateAttackMove(int directionX, int directionY)
{
	drawXRelative = 0;
	drawYRelative = 0;

	if (directionX > 0)
		drawXRelative = 6;
	else if (directionX < 0)
		drawXRelative = -6;

	if (directionY > 0)
		drawYRelative = 6;
	else if (directionY < 0)
		drawYRelative = -6;
}

int BaseLifeform::getFreeDroppingSpace(int& returnX, int& returnY, int& returnRoom, std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		if (returnRoom == -1 &&
			roomArray[i]->isInRoomAndDoorway(returnX, returnY) == true)
			returnRoom = i;

		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (roomArray[i]->tileArray[j]->x == returnX &&
				roomArray[i]->tileArray[j]->y == returnY)
				return false;
		}

		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			if (roomArray[i]->roomTileArray[j]->x == returnX &&
				roomArray[i]->roomTileArray[j]->y == returnY)
				return false;
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (roomArray[i]->hallwayArray[j]->x == returnX &&
				roomArray[i]->hallwayArray[j]->y == returnY)
				return false;
		}

		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (roomArray[i]->growingPlantArray[j]->exists == true &&
				roomArray[i]->growingPlantArray[j]->x == returnX &&
				roomArray[i]->growingPlantArray[j]->y == returnY)
				return false;
		}

		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (roomArray[i]->actorArray[j]->status.exists == true &&
				roomArray[i]->actorArray[j]->x == returnX &&
				roomArray[i]->actorArray[j]->y == returnY)
				return false;
		}

		for (int j = 0; j < roomArray[i]->itemArray.size(); ++j)
		{
			if (roomArray[i]->itemArray[j]->x == returnX &&
				roomArray[i]->itemArray[j]->y == returnY)
				return false;
		}
	}

	if (returnRoom == -1)
		returnRoom = 0;

	if (returnRoom > -1)
		return true;

	return false;
}

void BaseLifeform::blockPushback(int targetX, int targetY, int& myMoveX, int& myMoveY, int& targetMoveX, int& targetMoveY, std::vector<std::unique_ptr<Room>>& roomArray)
{
	if (facing == facingUpLeft)//get pushed opposite of facing, facing doesnt change
	{
		if (isTileFree(x + 1, y + 1, roomArray) == true)
		{
			myMoveX = 1;
			myMoveY = 1;
		}

		if (isTileFree(targetX - 1, targetY - 1, roomArray) == true)
		{
			targetMoveX = -1;
			targetMoveY = -1;
		}
	}

	else if (facing == facingUpRight)
	{
		if (isTileFree(x - 1, y + 1, roomArray) == true)
		{
			myMoveX = -1;
			myMoveY = 1;
		}

		if (isTileFree(targetX + 1, targetY - 1, roomArray) == true)
		{
			targetMoveX = 1;
			targetMoveY = -1;
		}
	}

	else if (facing == facingDownLeft)
	{
		if (isTileFree(x + 1, y - 1, roomArray) == true)
		{
			myMoveX = 1;
			myMoveY = -1;
		}

		if (isTileFree(targetX - 1, targetY + 1, roomArray) == true)
		{
			targetMoveX = -1;
			targetMoveY = 1;
		}
	}

	else if (facing == facingDownRight)
	{
		if (isTileFree(x - 1, y - 1, roomArray) == true)
		{
			myMoveX = -1;
			myMoveY = -1;
		}

		if (isTileFree(targetX + 1, targetY + 1, roomArray) == true)
		{
			targetMoveX = 1;
			targetMoveY = 1;
		}
	}

	else if (facing == facingUp)
	{
		if (isTileFree(x, y + 1, roomArray) == true)
			myMoveY = 1;

		if (isTileFree(targetX, targetY - 1, roomArray) == true)
			targetMoveY = -1;
	}

	else if (facing == facingLeft)
	{
		if (isTileFree(x + 1, y, roomArray) == true)
			myMoveX = 1;

		if (isTileFree(targetX - 1, targetY, roomArray) == true)
			targetMoveX = -1;
	}

	else if (facing == facingDown)
	{
		if (isTileFree(x, y - 1, roomArray) == true)
			myMoveY = -1;

		if (isTileFree(targetX, targetY + 1, roomArray) == true)
			targetMoveY = 1;
	}

	else if (facing == facingRight)
	{
		if (isTileFree(x - 1, y, roomArray) == true)
			myMoveX = -1;

		if (isTileFree(targetX + 1, targetY, roomArray) == true)
			targetMoveX = 1;
	}
}

int BaseLifeform::isTileFree(int checkX, int checkY, std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (roomArray[i]->tileArray[j]->x == checkX &&
				roomArray[i]->tileArray[j]->y == checkY)
			{
				return false;
			}
		}

		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)//traps don't block movement
			{
				if (roomArray[i]->roomTileArray[j]->x == checkX &&
					roomArray[i]->roomTileArray[j]->y == checkY)
				{
					return false;
				}
			}
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (roomArray[i]->hallwayArray[j]->x == checkX &&
				roomArray[i]->hallwayArray[j]->y == checkY)
			{
				return false;
			}
		}

		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (roomArray[i]->growingPlantArray[j]->exists == true &&
				roomArray[i]->growingPlantArray[j]->x == checkX &&
				roomArray[i]->growingPlantArray[j]->y == checkY)
			{
				return growingPlantBlocked;
			}
		}

		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (roomArray[i]->actorArray[j]->status.exists == true &&
				roomArray[i]->actorArray[j]->x == checkX &&
				roomArray[i]->actorArray[j]->y == checkY)
			{
				return false;
			}
		}
	}

	return true;
}

int BaseLifeform::isTileFreeDiagonal(int checkX1, int checkY1, int checkX2, int checkY2, int checkX3, int checkY3, std::vector<std::unique_ptr<Room>>& roomArray, bool checkDiagonal)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (roomArray[i]->tileArray[j]->x == checkX1 &&
				roomArray[i]->tileArray[j]->y == checkY1 ||
				roomArray[i]->tileArray[j]->x == checkX2 &&
				roomArray[i]->tileArray[j]->y == checkY2 ||
				roomArray[i]->tileArray[j]->x == checkX3 &&
				roomArray[i]->tileArray[j]->y == checkY3)
			{
				return false;
			}
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (roomArray[i]->hallwayArray[j]->x == checkX1 &&
				roomArray[i]->hallwayArray[j]->y == checkY1 ||
				roomArray[i]->hallwayArray[j]->x == checkX2 &&
				roomArray[i]->hallwayArray[j]->y == checkY2 ||
				roomArray[i]->hallwayArray[j]->x == checkX3 &&
				roomArray[i]->hallwayArray[j]->y == checkY3)
			{
				return false;
			}
		}

		if (checkDiagonal == true)
		{
			for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
			{
				if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)
				{
					if (roomArray[i]->roomTileArray[j]->x == checkX1 &&
						roomArray[i]->roomTileArray[j]->y == checkY1)
					{
						return false;
					}
				}
			}

			for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
			{
				if (roomArray[i]->growingPlantArray[j]->exists == true &&
					roomArray[i]->growingPlantArray[j]->x == checkX1 &&
					roomArray[i]->growingPlantArray[j]->y == checkY1)
				{
					return growingPlantBlocked;
				}
			}

			for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
			{
				if (roomArray[i]->actorArray[j]->status.exists == true &&
					roomArray[i]->actorArray[j]->x == checkX1 &&
					roomArray[i]->actorArray[j]->y == checkY1)
				{
					return false;
				}
			}
		}
	}

	return true;
}

int BaseLifeform::isTileACorner(int checkX, int checkY, int cornerBlockedTileX, int cornerBlockedTileY, std::vector<std::unique_ptr<Room>>& roomArray)
{
	bool cornerBlockedTileExists = false;

	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (roomArray[i]->tileArray[j]->x == checkX &&
				roomArray[i]->tileArray[j]->y == checkY)
			{
				return false;
			}

			else if (roomArray[i]->tileArray[j]->x == cornerBlockedTileX &&
				roomArray[i]->tileArray[j]->y == cornerBlockedTileY)
			{
				cornerBlockedTileExists = true;
			}
		}

		for (int j = 0; j < roomArray[i]->roomTileArray.size(); ++j)
		{
			if (roomArray[i]->roomTileArray[j]->tileType != tileTypeTrap)
			{
				if (roomArray[i]->roomTileArray[j]->x == checkX &&
					roomArray[i]->roomTileArray[j]->y == checkY)
				{
					return false;
				}

				if (roomArray[i]->roomTileArray[j]->x == cornerBlockedTileX &&
					roomArray[i]->roomTileArray[j]->y == cornerBlockedTileY)
				{
					cornerBlockedTileExists = true;
				}
			}
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (roomArray[i]->hallwayArray[j]->x == checkX &&
				roomArray[i]->hallwayArray[j]->y == checkY)
			{
				return false;
			}

			if (roomArray[i]->hallwayArray[j]->x == cornerBlockedTileX &&
				roomArray[i]->hallwayArray[j]->y == cornerBlockedTileY)
			{
				cornerBlockedTileExists = true;
			}
		}

		for (int j = 0; j < roomArray[i]->growingPlantArray.size(); ++j)
		{
			if (roomArray[i]->growingPlantArray[j]->exists == true &&
				roomArray[i]->growingPlantArray[j]->x == checkX &&
				roomArray[i]->growingPlantArray[j]->y == checkY)
			{
				return growingPlantBlocked;
			}

			if (roomArray[i]->growingPlantArray[j]->x == cornerBlockedTileX &&
				roomArray[i]->growingPlantArray[j]->y == cornerBlockedTileY)
			{
				cornerBlockedTileExists = true;
			}
		}

		for (int j = 0; j < roomArray[i]->actorArray.size(); ++j)
		{
			if (roomArray[i]->actorArray[j]->status.exists == true)
			{
				if (roomArray[i]->actorArray[j]->x == checkX &&
					roomArray[i]->actorArray[j]->y == checkY)
				{
					return false;
				}

				if (roomArray[i]->actorArray[j]->x == cornerBlockedTileX &&
					roomArray[i]->actorArray[j]->y == cornerBlockedTileY)
				{
					cornerBlockedTileExists = true;
				}
			}
		}
	}

	if (cornerBlockedTileExists == true)
		return true;

	return false;
}

bool BaseLifeform::isInThisDistanceOfMe(int checkX, int checkY, int distance)
{
	if (checkX >= x - distance &&
		checkX <= x + distance &&
		checkY >= y - distance &&
		checkY <= y + distance)
	{
		return true;
	}

	return false;
}

int BaseLifeform::checkDiagonalSides(int checkX1, int checkY1, int checkX2, int checkY2, std::vector<std::unique_ptr<Room>>& roomArray)
{
	for (int i = 0; i < roomArray.size(); ++i)
	{
		for (int j = 0; j < roomArray[i]->tileArray.size(); ++j)
		{
			if (roomArray[i]->tileArray[j]->x == checkX1 &&
				roomArray[i]->tileArray[j]->y == checkY1 ||
				roomArray[i]->tileArray[j]->x == checkX2 &&
				roomArray[i]->tileArray[j]->y == checkY2)
			{
				return false;
			}
		}

		for (int j = 0; j < roomArray[i]->hallwayArray.size(); ++j)
		{
			if (roomArray[i]->hallwayArray[j]->x == checkX1 &&
				roomArray[i]->hallwayArray[j]->y == checkY1 ||
				roomArray[i]->hallwayArray[j]->x == checkX2 &&
				roomArray[i]->hallwayArray[j]->y == checkY2)
			{
				return false;
			}
		}
	}

	return true;
}

void BaseLifeform::drawStatusEffects(int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	status.drawStatusEffect(gameViewDrawX, gameViewDrawY, drawXRelative, drawYRelative, visible, stoppedTime, gRenderer, txSpriteSheet);
}