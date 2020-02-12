#pragma once

#include "baseThing.h"
#include "statusEffectHolder.h"
#include "text.h"

enum itemTypeList { seedItemType, cropItemType, herbItemType, equipItemType };
enum plantTypeList { carrotPlantType, tomatoPlantType, cucumberPlantType, cherryPlantType, bananaPlantType, applePlantType, orangePlantType, eggplantPlantType, wheatPlantType, oatPlantType, redPepperPlantType, yellowPepperPlantType, lemonPlantType, limePlantType, carambolaPlantType, cornPlantType, grapePlantType, maxPlantTypeListSize };
enum herbTypeList { tastyHerbType, hungryHerbType, healHerbType, hurtHerbType, identifyHerbType, destroyHerbType, rememberHerbType, forgetHerbType, maxHerbTypeListSize };
enum equipTypeList { wateringCanEquipType, hoeEquipType, pitchforkEquipType, maxEquipTypeListSize };
enum equipSkillList { abilitySaveEquipSkill, noStatEquipSkill, plantMasteryEquipSkill, seedSaveEquipSkill, itemSaveEquipSkill, cropSaveEquipSkill, fastGrowEquipSkill, cropFoodUpEquipSkill, plantHPUpEquipSkill, herbFoodUpEquipSkill, seedPlusEquipSkill, herbPlusEquipSkill, maxEquipSkillListSize };

class Item : public BaseThing
{
public:
	//object
	bool pickedUp;
	bool identified;//is the seed/crop identified
	int itemType;//what type of item it is
	int subType;//what type of seed/crop/herb/equip it is

	std::string name;
	std::string addName;//if the object type has its type in its name "x Seed", "Vitamin x"
	std::string description;
	std::string activeAbilityDescription;
	int strength;//quality of growth. influences stats and price/strength of equip
	int nutrition;
	int price;//buy price. sell price is this divided by something
	bool doubled;//has it has its use doubled by an equipSkill, then it can't do it again

	Item(int x, int y, itemTypeList itemType, int subType);//spawn item
	Item(int x, int y, int subType, int strength);//spawn crop
	Item(itemTypeList itemType, int subType);//spawn into inventory
	~Item();

	void free();

	void setType();//object and equip

	void setPlantTypeName();
	void setCropSprite();
	void identifyPlant(bool identified);

	void setHerbTypeName();
	void identifyHerb(bool identified);

	void setEquipType();
	void setSkillType();

	int calculateNutrition();
	int calculatePrice();

	void drawType(SDL_Renderer* gRenderer, LTexture* txSpriteSheet, SDL_Rect& gameView);

	//equip
	equipSkillList skill;
	std::string skillDescription;
	int skillChance;//chance to activate passive skill

	std::unique_ptr<Item> slottedCrop;

	void powerSave(std::vector<std::unique_ptr<std::string>>& memoryTextArray);
};