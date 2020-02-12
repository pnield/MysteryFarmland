#pragma once

#include "baseThing.h"
#include "item.h"

class GrowingPlant : public BaseThing
{
public:
	bool exists;
	bool alive;//has it expired from not being watered
	std::unique_ptr<Item> grownCrop;//crop that i'm growing
	int maxHealth;
	int health;
	int growthLimit;//normally limits growingPlant to grow to grade A at 500 turns
	int seasonGrowth;//how much it can grow this season before being stopped
	int growthSpeed;
	int growthTimer;//how much growthSpeed has elapsed, sets what stage it is
	int stage;//what stage of development its on, sets what is strength is
	bool harvestable;//does it drop a plant
	bool watered;//if youve watered your plant on this floor, each time grows it by 100, else it withers
	int strength;//the strength of the resultant crop

	SDL_Rect drawnEffectRect;//drawn crop effect
	double drawnEffectY;
	bool drawHeart;//does it animate drawing a heart for this cycle instead of crop

	GrowingPlant(int& x, int& y, plantTypeList cropSubType, bool identified);
	~GrowingPlant();

	void free();

	void setIdentification(bool identified);
	void setDrawnEffectType();

	void careForMe(bool watering);
	void setHeartDrawn();
	void takeDamage(int damageTaken, int rumbleTaken);
	void addHealth(int healthGained);
	void wither();

	void grow(int season);

	void drawEffect(int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
	void drawnEffectAnimate();
};