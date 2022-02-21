#include "unit.h"


UnitAttackConstParams::UnitAttackConstParams(unsigned int heal, unsigned int firstDamage, unsigned int firstInitiative, 
	TargetsCount targetsCount, AttackClass attackClass, bool infinite, std::string attackId, unsigned int level, AttackSource source)
{
	this->attackClass = attackClass;
	this->attackId = attackId;
	this->firstDamage = firstDamage;
	this->firstInitiative = firstInitiative;
	this->heal = heal;
	this->infinite = infinite;
	this->level = level;
	this->source = source;
	this->targetsCount = targetsCount;

}


UnitConstParams::UnitConstParams(unsigned int maxHp, std::string unitName, std::string unitGameId,
	std::string unitWarId, bool isDoubleAttack, unsigned int upgradeDamage, unsigned int upgradeArmor,
	unsigned int upgradeInitiative, unsigned int upgradeHeal, 
	unsigned int upgradePower, unsigned int upgradeHp, bool overlevel)
{
	this->isDoubleAttack = isDoubleAttack;
	this->maxHp = maxHp;
	this->unitGameId = unitGameId;
	this->unitName = unitName;
	this->unitWarId = unitWarId;
	this->upgradeArmor = upgradeArmor;
	this->upgradeDamage = upgradeDamage;
	this->upgradeHeal = upgradeHeal;
	this->upgradeHp = upgradeHp;
	this->upgradeInitiative = upgradeInitiative;
	this->upgradePower = upgradePower;
	this->overlevel = overlevel;
	
}


Unit::~Unit()
{
	delete attacksMap;
	delete hasClassImmune;
	delete hasSourceImmune;
	delete attack1;
	if (attack2) {
		delete attack2;
	}
}

Unit::Unit(UnitConstParams* unitConstParams, bool isMoving, bool isDead, bool isProtected, bool isWaiting, bool isBig,
	unsigned int currentHp, unsigned int currentAttack, UnitAttack* attack1, UnitAttack* attack2, std::unordered_map<AttackClass, 
	UnitAttack*>* attacksMap, unsigned int armor, bool retreat, bool paralyzed, bool pertified, bool poisoned, bool blistered, 
	bool frostbited, bool damageLower, bool initLower, bool revived, bool damageBusted, bool transformed, unsigned int level, 
	std::unordered_map<unsigned int, ImmuneCategory>* classImmune, std::unordered_map<unsigned int, ImmuneCategory>* sourceImmune, 
	std::unordered_map<unsigned int, bool>* hasClassImmune, std::unordered_map<unsigned int, bool>* hasSourceImmune, bool isEmpty)
{
	this->attack1 = attack1;
	this->attack2 = attack2;
	this->armor = armor;
	this->attacksMap = attacksMap;
	this->blistered = blistered;
	this->classImmune = classImmune;
	this->currentAttack = currentAttack;
	this->currentHp = currentHp;
	this->damageBusted = damageBusted;
	this->damageLower = damageLower;
	this->frostbited = frostbited;
	this->hasClassImmune = hasClassImmune;
	this->hasSourceImmune = hasSourceImmune;
	this->initLower = initLower;
	this->isBig = isBig;
	this->isDead = isDead;
	this->isMoving = isMoving;
	this->isProtected = isProtected;
	this->isWaiting = isWaiting;
	this->level = level;
	this->paralyzed = paralyzed;
	this->petrified = pertified;
	this->poisoned = poisoned;
	this->retreat = retreat;
	this->revived = revived;
	this->sourceImmune = sourceImmune;
	this->transformed = transformed;
	this->unitConstParams = unitConstParams;
	this->isEmpty = isEmpty;
}

Unit* Unit::deepCopy()
{
	
	if (isEmpty) {
		return this;
	}

	auto newAttacksMap = new std::unordered_map<AttackClass, UnitAttack*>{};

	//auto newClassImmune = new std::unordered_map<unsigned int, ImmuneCategory>{};
	//auto newSourceImmune = new std::unordered_map<unsigned int, ImmuneCategory>{};
	auto newHasClassImmune = new std::unordered_map<unsigned int, bool>{};
	auto newHasSourceImmune = new std::unordered_map<unsigned int, bool>{};

	for (auto pair : *this->hasClassImmune) {
		(*newHasClassImmune)[pair.first] = pair.second;
	}
	for (auto pair : *this->hasSourceImmune) {
		(*newHasSourceImmune)[pair.first] = pair.second;
	}
	for (auto pair : *this->attacksMap) {
		(*newAttacksMap)[pair.first] = pair.second->deepCopy();
	}

	auto newAttack2 = this->attack2 ? attack2->deepCopy() : nullptr;

	auto copy = new Unit(this->unitConstParams,
		this->isMoving,
		this->isDead,
		this->isProtected,
		this->isWaiting,
		this->isBig,
		this->currentHp,
		this->currentAttack,
		this->attack1->deepCopy(),
		newAttack2,
		newAttacksMap,
		this->armor,
		this->retreat,
		this->paralyzed,
		this->petrified,
		this->poisoned,
		this->blistered,
		this->frostbited,
		this->damageLower,
		this->initLower,
		this->revived,
		this->damageBusted,
		this->transformed,
		this->level,
		this->classImmune,
		this->sourceImmune,
		newHasClassImmune,
		newHasSourceImmune,
		this->isEmpty);

	return copy;
}


UnitAttack::UnitAttack(unsigned int power, unsigned int damage, unsigned int initiative, unsigned int currentDuration, UnitAttackConstParams* constParams)
{
	this->constParams = constParams;
	this->currentDuration = currentDuration;
	this->damage = damage;
	this->initiative = initiative;
	this->power = power;

}

UnitAttack* UnitAttack::deepCopy()
{

	auto copy = new UnitAttack(
		this->power,
		this->damage,
		this->initiative,
		this->currentDuration,
		this->constParams
	);


	return copy;
}
