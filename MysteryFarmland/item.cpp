#include "item.h"

Item::Item(int x, int y, itemTypeList itemType, int subType)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();

	pickedUp = false;
	identified = false;
	this->itemType = itemType;
	this->subType = subType;
	strength = 0;
	doubled = false;

	setType();//set object name, sprite based on objectType
}

Item::Item(int x, int y, int subType, int strength)
{
	this->x = x;
	this->y = y;
	lastX = x;
	lastY = y;
	setDrawXY();
	pickedUp = false;
	itemType = cropItemType;
	this->subType = subType;
	this->strength = strength;
	doubled = false;

	setType();
}

Item::Item(itemTypeList itemType, int subType)
{
	x = 0;
	y = 0;
	pickedUp = true;
	this->itemType = itemType;
	this->subType = subType;
	strength = 0;
	doubled = false;

	setType();

}

Item::~Item()
{
	free();
}

void Item::free()
{
	slottedCrop.reset();
}

void Item::setType()
{
	if (itemType == seedItemType)
	{
		addName = " Seed";
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 80;
		setPlantTypeName();
		price = 20;
	}

	else if (itemType == cropItemType)
	{
		setPlantTypeName();
		setCropSprite();
		nutrition = calculateNutrition();
		price = calculatePrice();
		addName = "";
	}

	else if (itemType == herbItemType)
	{
		addName = " Herb";
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 80;
		setHerbTypeName();
		nutrition = 10;

		if (subType == tastyHerbType)
			nutrition = 20;
	}

	else if (itemType == equipItemType)
	{
		addName = "";
		description = "A farming tool";
		identified = true;
		setEquipType();
		price = 40;
	}
}

void Item::setPlantTypeName()
{
	if (identified == false)
	{
		name = "[???]";
		if (itemType == seedItemType)
			description = "Unidentified Seed";
		else
			description = "Unidentified Crop";
		activeAbilityDescription = "";
	}

	else if (subType == carrotPlantType)//if the player has identified it
	{
		name = "[Carrot]";
		description = "Teleports you";
		activeAbilityDescription = "Teleport you home";
	}

	else if (subType == tomatoPlantType)
	{
		name = "[Tomato]";
		description = "Discover traps";
		activeAbilityDescription = "Discover farm";
	}

	else if (subType == cucumberPlantType)
	{
		name = "[Cucumber]";
		description = "Fly backwards";
		activeAbilityDescription = "Repel enemies";
	}

	else if (subType == cherryPlantType)
	{
		name = "[Cherry]";
		description = "Explodes";
		activeAbilityDescription = "Explode enemies";
	}

	else if (subType == bananaPlantType)
	{
		name = "[Banana]";
		description = "Sets trap";
		activeAbilityDescription = "Trap resistant";
	}

	else if (subType == applePlantType)
	{
		name = "[Apple]";
		description = "Puts you to sleep";
		activeAbilityDescription = "Put enemies to sleep";
	}

	else if (subType == orangePlantType)
	{
		name = "[Orange]";
		description = "Brings animal";
		activeAbilityDescription = "Summon animal";
	}

	else if (subType == eggplantPlantType)
	{
		name = "[Eggplant]";
		description = "Poisons you";
		activeAbilityDescription = "Poison enemies";
	}

	else if (subType == wheatPlantType)
	{
		name = "[Wheat]";
		description = "Cure status effects";
		activeAbilityDescription = "Resist status effects";
	}

	else if (subType == oatPlantType)
	{
		name = "[Oat]";
		description = "Restores stamina";
		activeAbilityDescription = "Restore stamina";
	}

	else if (subType == redPepperPlantType)
	{
		name = "[Red Pepper]";
		description = "Burns you";
		activeAbilityDescription = "Shoot fireball";
	}

	else if (subType == yellowPepperPlantType)
	{
		name = "[Ylw Pepper]";
		description = "Shocks you";
		activeAbilityDescription = "Shoot electricity";
	}

	else if (subType == lemonPlantType)
	{
		name = "[Lemon]";
		description = "Evolves enemy";
		activeAbilityDescription = "Devolve enemies";
	}

	else if (subType == limePlantType)
	{
		name = "[Lime]";
		description = "Wakes enemy";
		activeAbilityDescription = "Sleep resistant";
	}

	else if (subType == carambolaPlantType)
	{
		name = "[Carambola]";
		description = "Stops time briefly";
		activeAbilityDescription = "Stop time";
	}

	else if (subType == cornPlantType)
	{
		name = "[Corn]";
		description = "Shrinks you";
		activeAbilityDescription = "Shrink enemies";
	}

	else if (subType == grapePlantType)
	{
		name = "[Grape]";
		description = "Confuses you";
		activeAbilityDescription = "Confuse enemies";
	}
}

void Item::setCropSprite()
{
	if (identified == false)
	{
		if (strength == 0)
		{
			spriteSheetRect.x = 32;
			spriteSheetRect.y = 0;
		}

		else if (strength == 1)
		{
			spriteSheetRect.x = 40;
			spriteSheetRect.y = 0;
		}

		else if (strength == 2)
		{
			spriteSheetRect.x = 48;
			spriteSheetRect.y = 0;
		}

		else if (strength == 3)
		{
			spriteSheetRect.x = 56;
			spriteSheetRect.y = 0;
		}
	}

	else if (subType == carrotPlantType)
	{
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 8;
	}

	else if (subType == tomatoPlantType)
	{
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 8;
	}

	else if (subType == cucumberPlantType)
	{
		spriteSheetRect.x = 48;
		spriteSheetRect.y = 8;
	}

	else if (subType == cherryPlantType)
	{
		spriteSheetRect.x = 56;
		spriteSheetRect.y = 8;
	}

	else if (subType == bananaPlantType)
	{
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 16;
	}

	else if (subType == applePlantType)
	{
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 16;
	}

	else if (subType == orangePlantType)
	{
		spriteSheetRect.x = 48;
		spriteSheetRect.y = 16;
	}

	else if (subType == eggplantPlantType)
	{
		spriteSheetRect.x = 56;
		spriteSheetRect.y = 16;
	}

	else if (subType == wheatPlantType)
	{
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 24;
	}

	else if (subType == oatPlantType)
	{
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 24;
	}

	else if (subType == redPepperPlantType)
	{
		spriteSheetRect.x = 48;
		spriteSheetRect.y = 24;
	}

	else if (subType == yellowPepperPlantType)
	{
		spriteSheetRect.x = 56;
		spriteSheetRect.y = 24;
	}

	else if (subType == lemonPlantType)
	{
		spriteSheetRect.x = 64;
		spriteSheetRect.y = 0;
	}

	else if (subType == limePlantType)
	{
		spriteSheetRect.x = 72;
		spriteSheetRect.y = 0;
	}

	else if (subType == carambolaPlantType)
	{
		spriteSheetRect.x = 80;
		spriteSheetRect.y = 0;
	}

	else if (subType == cornPlantType)
	{
		spriteSheetRect.x = 88;
		spriteSheetRect.y = 0;
	}

	else if (subType == grapePlantType)
	{
		spriteSheetRect.x = 64;
		spriteSheetRect.y = 8;
	}
}

void Item::identifyPlant(bool identified)
{
	this->identified = identified;

	setPlantTypeName();

	if (itemType == cropItemType)
		setCropSprite();
}

void Item::setHerbTypeName()
{
	if (identified == false)
	{
		name = "[???]";
		description = "Unidentified Herb";
	}

	else if (subType == tastyHerbType)
	{
		name = "[Tasty]";
		description = "Tastes good";
	}

	else if (subType == hungryHerbType)
	{
		name = "[Hungry]";
		description = "Tastes bad";
	}

	else if (subType == healHerbType)
	{
		name = "[Heal]";
		description = "Heals you";
	}

	else if (subType == hurtHerbType)
	{
		name = "[Hurt]";
		description = "Hurts you";
	}

	else if (subType == identifyHerbType)
	{
		name = "[Identify]";
		description = "Identifies item";
	}

	else if (subType == destroyHerbType)
	{
		name = "[Destroy]";
		description = "Destroys item";
	}

	else if (subType == rememberHerbType)
	{
		name = "[Remember]";
		description = "Remember something";
	}

	else if (subType == forgetHerbType)
	{
		name = "[Forget]";
		description = "Forget something";
	}
}

void Item::identifyHerb(bool identified)
{
	this->identified = identified;

	setHerbTypeName();
}

void Item::setEquipType()
{
	if (subType == wateringCanEquipType)
	{
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 0;
		name = "[Watering Can]";
		skill = (equipSkillList)getRandomInt(2, maxEquipSkillListSize - 1);
		setSkillType();
	}

	else if (subType == hoeEquipType)
	{
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 8;
		name = "[Hoe]";
		skill = abilitySaveEquipSkill;
		setSkillType();
	}

	else if (subType == pitchforkEquipType)
	{
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 16;
		name = "[Pitchfork]";
		slottedCrop = std::make_unique<Item>(cropItemType, getRandomInt(0, maxPlantTypeListSize - 1));
		slottedCrop->identifyPlant(true);
		strength = getRandomInt(1, 4);
		skill = noStatEquipSkill;
		setSkillType();
	}
}

void Item::setSkillType()
{
	if (skill == noStatEquipSkill)
		skillDescription = "No stat gain";

	else if (skill == abilitySaveEquipSkill)
	{
		skillDescription = "Power saver";
		skillChance = 25;
	}

	else if (skill == plantMasteryEquipSkill)
		skillDescription = "Skilled farming";

	else if (skill == seedSaveEquipSkill)
	{
		skillDescription = "Seed conservation";
		skillChance = 25;
	}

	else if (skill == itemSaveEquipSkill)
	{
		skillDescription = "Herb rationing";
		skillChance = 25;
	}

	else if (skill == cropSaveEquipSkill)
	{
		skillDescription = "Crop rationing";
		skillChance = 25;
	}

	else if (skill == fastGrowEquipSkill)
		skillDescription = "Fast growing crops";

	else if (skill == cropFoodUpEquipSkill)
		skillDescription = "Tastier crops";

	else if (skill == plantHPUpEquipSkill)
		skillDescription = "Tougher crops";

	else if (skill == herbFoodUpEquipSkill)
		skillDescription = "Tastier herbs";

	else if (skill == seedPlusEquipSkill)
		skillDescription = "Seed finder";

	else if (skill == herbPlusEquipSkill)
		skillDescription = "Herb finder";
}

int Item::calculateNutrition()
{
	if (strength == 0)
		return 10;
	else if (strength == 1)
		return 20;
	else
		return 50;
}

int Item::calculatePrice()
{
	if (strength == 0)
		return 5;
	else if (strength == 1)
		return 10;
	else if (strength == 2)
		return 20;
	else
		return 100;
}

void Item::drawType(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView)
{
	if (pickedUp == false)
		drawMapPosition(gRenderer, txSpriteSheet, gameView);
	else
		drawRealPosition(gRenderer, txSpriteSheet);
}

void Item::powerSave(std::vector<std::unique_ptr<std::string>>& memoryTextArray)
{
	if(skill == abilitySaveEquipSkill && 
		getRandomInt(1, 100) <= skillChance)
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Power charge saved!")));
	else
	{
		memoryTextArray.insert(memoryTextArray.begin(), std::move(std::make_unique<std::string>("Power charge spent.")));
		strength--;
	}
}