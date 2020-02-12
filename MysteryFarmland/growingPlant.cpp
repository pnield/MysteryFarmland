#include "growingPlant.h"

GrowingPlant::GrowingPlant(int& x, int& y, plantTypeList cropSubType, bool identified)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();
	exists = true;
	alive = true;

	grownCrop = std::make_unique<Item>((itemTypeList)cropItemType, cropSubType);
	setIdentification(identified);

	maxHealth = 2;
	health = maxHealth;
	growthLimit = 500;
	seasonGrowth = 0;
	growthSpeed = 1;
	growthTimer = 0;
	stage = 0;
	harvestable = false;
	watered = false;
	strength = 0;

	drawnEffectRect.w = 8;
	drawnEffectRect.h = 8;
	drawnEffectY = 2;
	drawHeart = false;

	spriteSheetRect.x = 40;
	spriteSheetRect.y = 64;
}

GrowingPlant::~GrowingPlant()
{
	free();
}

void GrowingPlant::free()
{
	grownCrop.reset();
}

void GrowingPlant::setIdentification(bool identified)
{
	grownCrop->identifyPlant(identified);
	setDrawnEffectType();
}

void GrowingPlant::setDrawnEffectType()
{
	drawnEffectRect.x = grownCrop->spriteSheetRect.x;
	drawnEffectRect.y = grownCrop->spriteSheetRect.y;
}

void GrowingPlant::setHeartDrawn()
{
	drawHeart = true;
	drawnEffectRect.x = 32;
	drawnEffectRect.y = 32;
	drawnEffectY = 0;
}

void GrowingPlant::careForMe(bool watering)
{
	if (watering == true)
		watered = true;

	setHeartDrawn();
	growthTimer += 100;
	addHealth(1);
}

void GrowingPlant::takeDamage(int damageTaken, int rumbleTaken)
{
	health -= damageTaken;
	rumble = rumbleTaken;

	if (health <= 0)
		exists = false;
}

void GrowingPlant::addHealth(int healthGained)
{
	if (alive == true)
	{
		health += healthGained;

		if (health > maxHealth)
			health = maxHealth;
	}
}

void GrowingPlant::wither()
{
	rumble = 5;
	spriteSheetRect.x = 32;
	spriteSheetRect.y = 72;

	alive = false;
	health = 1;
}

void GrowingPlant::grow(int season)
{
	if (alive == true)
	{
		if (growthTimer < growthLimit ||
			stage < 4)
		{
			if (seasonGrowth < 100)
			{
				seasonGrowth++;
				growthTimer += growthSpeed;
			}

			if (growthTimer >= 100 &&
				stage < 1)
			{
				stage = 1;
				spriteSheetRect.x = 48;
				spriteSheetRect.y = 64;
				rumble = 2;
			}

			if (growthTimer >= 150 &&
				stage < 2)
			{
				stage = 2;
				spriteSheetRect.x = 56;
				spriteSheetRect.y = 64;
				harvestable = true;
				rumble = 2;
			}

			if (growthTimer >= 300 &&
				stage < 3)
			{
				stage = 3;
				spriteSheetRect.x = 40;
				spriteSheetRect.y = 72;
				strength++;
				rumble = 2;
			}

			if (growthTimer >= 500 &&
				stage < 4)
			{
				stage = 4;
				spriteSheetRect.x = 48;
				spriteSheetRect.y = 72;
				strength++;
				rumble = 2;
			}
		}

		if (growthTimer >= 1000 &&
			stage < 5)
		{
			stage = 5;
			spriteSheetRect.x = 56;
			spriteSheetRect.y = 72;
			strength++;
			rumble = 2;
		}
	}
}

void GrowingPlant::drawEffect(int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	if (alive == true)
	{
		if (grownCrop->identified == true ||
			drawHeart == true)
		{
			if (visible == true)
				txSpriteSheet->renderClip(round(gameViewDrawX + drawXRelative), round(gameViewDrawY + drawYRelative + drawnEffectY), gRenderer, drawnEffectRect);

			if (stoppedTime == false)
				drawnEffectAnimate();
		}
	}
}

void GrowingPlant::drawnEffectAnimate()
{
	drawnEffectY -= 0.25;

	if (drawnEffectY < -8)
	{
		drawnEffectY = 2;

		if (drawHeart == true)
		{
			drawHeart = false;
			setDrawnEffectType();
		}
	}
}