#pragma once

#include "lTexture.h"

enum statusEffectList { noStatusEffect = -3, stopTimeStatusEffect = -2, friendlyStatusEffect = -1, asleepStatusEffect, blockingStatusEffect, poisonedStatusEffect, fleeingStatusEffect, stunnedStatusEffect, shrunkenStatusEffect, confuseStatusEffect, hastenedStatusEffect, slowedStatusEffect, maxStatusEffectListSize};

class StatusEffectHolder
{
public:
	SDL_Rect spriteSheetRect;//only one status effect drawn at a time
	double statusEffectY;
	statusEffectList animatedStatusEffect;//what status effect is currently animated

	bool exists;//if false, delete it
	bool friendly;//if an actor is friendly
	int asleep;//int to show its timer. 0 is unaffected, > 0 is affected and counting down
	int blocking;//reduce damage by half for player, reduce damage taken to 1 for actor, pushes attacker away by one backwards if theres space
	int poisoned;//poisoned, lose health and can't regenerate it
	int fleeing;//flees from enemies at double speed
	int stunned;//misses one turn
	int shrunken;//max hp and damage is halved, can displace enemies
	int confused;//enemies move randomly, attack their own and cant use abilities. player moves and attacks in random directions when attempted
	int hastened;//sped up and able to move twice per turn
	int slowed;//slowed down to only be able to move every other turn
	bool missThisTurn;//if slowed, miss every other turn
	
	bool resistStatusEffects;//cant be affected by other status effects or traps
	bool fastHunger;//satiety decreases quicker
	bool fastHealthRegen;//health restores quicker
	bool seeAll;//tomato power
	int stopTime;//no one but player can act. status effects dont move

	void initializeStatusEffects();
	void removeAllStatusEffects();
	void removeNegativeStatusEffects();
	void addStatusEffect(statusEffectList statusAdded, int timeAdded);
	bool hasNegativeStatusEffect();

	void applyStatus(int& health, bool fatalPoison);//apply poison if true and checks if youre alive
		void checkDeath(int& health);//so that if an actor is killed after its own turn during another actor turn, it won't stay alive until its next turn
	void timeDownStatusEffects();//reduce status effects to 0 over time

	void drawStatusEffect(int& gameViewDrawX, int& gameViewDrawY, int& drawXRelative, int& drawYRelative, bool visible, int stoppedTime, SDL_Renderer* gRenderer, LTexture* txSpriteSheet);
	void statusEffectAnimate(bool stoppedTime);
};