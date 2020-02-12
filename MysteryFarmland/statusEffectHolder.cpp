#include "statusEffectHolder.h"

void StatusEffectHolder::initializeStatusEffects()
{
	spriteSheetRect.w = 8;
	spriteSheetRect.h = 8;
	statusEffectY = 2;
	animatedStatusEffect = noStatusEffect;
	
	exists = true;
	friendly = false;

	removeAllStatusEffects();
}

void StatusEffectHolder::removeAllStatusEffects()
{
	blocking = false;
	asleep = false;
	poisoned = false;
	fleeing = false;
	stunned = false;
	shrunken = false;
	confused = false;
	hastened = false;
	slowed = false;

	resistStatusEffects = false;
	fastHunger = false;
	fastHealthRegen = false;
	seeAll = false;
	stopTime = false;
}

void StatusEffectHolder::removeNegativeStatusEffects()
{
	asleep = false;
	poisoned = false;
	fleeing = false;
	stunned = false;
	shrunken = false;
	confused = false;
	slowed = false;

	fastHunger = false;
}

void StatusEffectHolder::addStatusEffect(statusEffectList statusAdded, int timeAdded)
{
	timeAdded++;

	if (resistStatusEffects == false)
	{
		if (statusAdded == friendlyStatusEffect)
			friendly = true;
		else if (statusAdded == blockingStatusEffect)
			blocking = timeAdded;
		else if (statusAdded == asleepStatusEffect)
			asleep = timeAdded;
		else if (statusAdded == poisonedStatusEffect)
			poisoned = timeAdded;
		else if (statusAdded == fleeingStatusEffect)
			fleeing = timeAdded;
		else if (statusAdded == stunnedStatusEffect)
			stunned = timeAdded;
		else if (statusAdded == shrunkenStatusEffect)
			shrunken = timeAdded;
		else if (statusAdded == confuseStatusEffect)
			confused = timeAdded;
		else if (statusAdded == hastenedStatusEffect)
			hastened = timeAdded;
		else if (statusAdded == slowedStatusEffect)
			slowed = timeAdded;
	}

	if (statusAdded == stopTimeStatusEffect)
		stopTime = timeAdded;
}

bool StatusEffectHolder::hasNegativeStatusEffect()
{
	if (asleep >= true ||
		poisoned >= true ||
		fleeing >= true ||
		stunned >= true ||
		shrunken >= true ||
		confused >= true ||
		slowed >= true)
		return true;

	return false;
}

void StatusEffectHolder::applyStatus(int& health, bool fatalPoison)
{
	if (slowed >= true)
		missThisTurn = !missThisTurn;
	else
		missThisTurn = false;

	if (missThisTurn == false)
	{
		if (poisoned >= true)
		{
			if (fatalPoison == true)
				health--;
			else if (health > 1)
				health--;
		}

		checkDeath(health);
	}
}

void StatusEffectHolder::checkDeath(int& health)
{
	if (health <= 0)
		exists = false;
}

void StatusEffectHolder::timeDownStatusEffects()
{
	if (blocking > 0 &&
		blocking < 999)
		blocking--;

	if (asleep > 0 &&
		asleep < 999)
		asleep--;

	if (poisoned > 0 &&
		poisoned < 999)
		poisoned--;

	if (fleeing > 0 &&
		fleeing < 999)
		fleeing--;

	if (stunned > 0 &&
		stunned < 999)
		stunned--;

	if (shrunken > 0 &&
		shrunken < 999)
		shrunken--;

	if (confused > 0  &&
		confused < 999)
		confused--;

	if (hastened > 0 &&
		hastened < 999)
		hastened--;

	if (slowed > 0 &&
		slowed < 999)
		slowed--;

	if(stopTime > 0)
		stopTime--;
}

void StatusEffectHolder::drawStatusEffect(int& gameViewDrawX, int& gameViewDrawY, int& drawXRelative, int& drawYRelative, bool visible, int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet)
{
	int statusEffectYBase = 2;

	if (visible >= true)
	{
		bool setStatusEffect = true;

		while (setStatusEffect == true)
		{
			if (friendly == true &&
				animatedStatusEffect == noStatusEffect)//friendly
			{
				animatedStatusEffect = friendlyStatusEffect;
				spriteSheetRect.x = 32;
				spriteSheetRect.y = 32;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == friendlyStatusEffect)
			{
				if (friendly == true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (asleep >= true &&
				animatedStatusEffect == noStatusEffect)//asleep
			{
				animatedStatusEffect = asleepStatusEffect;

				if (asleep >= 999)
				{
					spriteSheetRect.x = 40;
					spriteSheetRect.y = 32;
				}

				else
				{
					spriteSheetRect.x = 48;
					spriteSheetRect.y = 32;
				}

				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == asleepStatusEffect)
			{
				if (asleep >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (blocking >= true &&
				animatedStatusEffect == noStatusEffect)//blocking
			{
				animatedStatusEffect = blockingStatusEffect;
				spriteSheetRect.x = 56;
				spriteSheetRect.y = 32;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == blockingStatusEffect)
			{
				if (blocking >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (poisoned >= true &&
				animatedStatusEffect == noStatusEffect)//poison
			{
				animatedStatusEffect = poisonedStatusEffect;
				spriteSheetRect.x = 32;
				spriteSheetRect.y = 40;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == poisonedStatusEffect)
			{
				if (poisoned >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (fleeing >= true &&
				animatedStatusEffect == noStatusEffect)//fleeing
			{
				animatedStatusEffect = fleeingStatusEffect;
				spriteSheetRect.x = 40;
				spriteSheetRect.y = 40;
				statusEffectY = -8;
			}

			if (animatedStatusEffect == fleeingStatusEffect)
			{
				if (fleeing >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (stunned >= true &&
				animatedStatusEffect == noStatusEffect)//stunned
			{
				animatedStatusEffect = stunnedStatusEffect;
				spriteSheetRect.x = 16;
				spriteSheetRect.y = 56;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == stunnedStatusEffect)
			{
				if (stunned >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (confused >= true &&
				animatedStatusEffect == noStatusEffect)//confuse
			{
				animatedStatusEffect = confuseStatusEffect;
				spriteSheetRect.x = 48;
				spriteSheetRect.y = 40;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == confuseStatusEffect)
			{
				if (confused >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (hastened >= true &&
				animatedStatusEffect == noStatusEffect)//hastened
			{
				animatedStatusEffect = hastenedStatusEffect;
				spriteSheetRect.x = 56;
				spriteSheetRect.y = 40;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == hastenedStatusEffect)
			{
				if (hastened >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			if (slowed >= true &&
				animatedStatusEffect == noStatusEffect)//slowed
			{
				animatedStatusEffect = slowedStatusEffect;
				spriteSheetRect.x = 32;
				spriteSheetRect.y = 48;
				statusEffectY = statusEffectYBase;
			}

			if (animatedStatusEffect == slowedStatusEffect)
			{
				if (slowed >= true)
					statusEffectAnimate(stoppedTime);
				else
					animatedStatusEffect = noStatusEffect;
			}

			setStatusEffect = false;

			if (animatedStatusEffect == noStatusEffect)//if animatable status effect is true but its not animating
			{
				if (friendly == true ||
					blocking >= true ||
					asleep >= true ||
					poisoned >= true ||
					fleeing >= true ||
					stunned >= true ||
					confused >= true ||
					hastened >= true ||
					slowed >= true)
					setStatusEffect = true;//set a status effect sprite
			}
		}
	}

	else
		animatedStatusEffect = noStatusEffect;

	if (animatedStatusEffect != noStatusEffect)
		txSpriteSheet->renderClip(round(gameViewDrawX + drawXRelative), round(gameViewDrawY + drawYRelative + statusEffectY), gRenderer, spriteSheetRect);
}

void StatusEffectHolder::statusEffectAnimate(bool stoppedTime)
{
	if (stoppedTime == false)
	{
		if (animatedStatusEffect != fleeingStatusEffect)
			statusEffectY -= 0.25;
		else
			statusEffectY += 0.25;

		if (animatedStatusEffect != fleeingStatusEffect)
		{
			if (statusEffectY < -8)
				animatedStatusEffect = noStatusEffect;
		}

		else
		{
			if (statusEffectY > 2)
				animatedStatusEffect = noStatusEffect;
		}
	}
}