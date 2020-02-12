#pragma once

#include "baseThing.h"
#include "statusEffectHolder.h"

class Room;

enum facingList { facingLeft, facingUpLeft, facingUp, facingUpRight, facingRight, facingDownRight, facingDown, facingDownLeft, maxFacingListSize };
enum growingPlantBlockedEnum { growingPlantBlocked = 2 };

class BaseLifeform : public BaseThing
{
public:
	std::string name;
	StatusEffectHolder status;

	int facing;//faced direction
	int baseMaxHealth;
	int maxHealth;
	int health;
	double baseAttackPower;
	double baseDefensePower;
	int dodgeChance;//% chance to avoid defendable damage

	BaseLifeform();

	int generateAttackDamage(double attackPower);
	bool takeDefendableDamage(int& damageTaken);
	void takeTrueDamage(int damageTaken);
	void addHealth(int addAmount);
	void setFacing(int facingX, int facingY, bool animateAttack);//sets facing towards given position
	void targetFacing(int& targetX, int& targetY, bool animateAttack);//returns a +/- 1 value towards faced direction
	void animateAttackMove(int directionX, int directionY);
	int getFreeDroppingSpace(int& returnX, int& returnY, int& returnRoom, std::vector<std::unique_ptr<Room>>& roomArray);
	void blockPushback(int targetX, int targetY, int& myMoveX, int& myMoveY, int& targetMoveX, int& targetMoveY, std::vector<std::unique_ptr<Room>>& roomArray);

	int isTileFree(int checkX, int checkY, std::vector<std::unique_ptr<Room>>& roomArray);
	int isTileFreeDiagonal(int checkX1, int checkY1, int checkX2, int checkY2, int checkX3, int checkY3, std::vector<std::unique_ptr<Room>>& roomArray, bool checkDiagonal);
	int isTileACorner(int checkX, int checkY, int cornerBlockedTileX, int cornerBlockedTileY, std::vector<std::unique_ptr<Room>>& roomArray);
	bool isInThisDistanceOfMe(int checkX, int checkY, int distance);
	int checkDiagonalSides(int checkX1, int checkY1, int checkX2, int checkY2, std::vector<std::unique_ptr<Room>>& roomArray);

	void drawStatusEffects(int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
};