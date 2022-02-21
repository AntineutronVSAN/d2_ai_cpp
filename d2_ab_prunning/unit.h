#pragma once

#include <string>
#include <unordered_map>

enum class ImmuneCategory;
enum class AttackClass;
enum class TargetsCount;
enum class AttackSource;

const std::string EMPTY_UNIT_IDS = "EMPTY";

class UnitConstParams
{
public:
	UnitConstParams(unsigned int maxHp,
		std::string unitName,
		std::string unitGameId,
		std::string unitWarId,

		bool isDoubleAttack,

		unsigned unsigned int upgradeDamage,
		unsigned int upgradeArmor,
		unsigned int upgradeInitiative,
		unsigned int upgradeHeal,
		unsigned int upgradePower,
		unsigned int upgradeHp,
		bool overlevel
	);

	unsigned int maxHp;
	std::string unitName;
	std::string unitGameId;
	std::string unitWarId;

	bool isDoubleAttack;

	unsigned int upgradeDamage;
	unsigned int upgradeArmor;
	unsigned int upgradeInitiative;
	unsigned int upgradeHeal;
	unsigned int upgradePower;
	unsigned int upgradeHp;
	bool overlevel;

};

class UnitAttackConstParams
{
public:
	UnitAttackConstParams(unsigned int heal,
		unsigned int firstDamage,
		unsigned int firstInitiative,
		TargetsCount targetsCount,
		AttackClass attackClass,
		bool infinite,
		std::string attackId,
		unsigned int level,
		AttackSource source);

	unsigned int heal;
	unsigned int firstDamage;
	unsigned int firstInitiative;
	TargetsCount targetsCount;
	AttackClass attackClass;
	bool infinite;
	std::string attackId;
	unsigned int level;
	AttackSource source;
};

class UnitAttack
{
public:
	UnitAttack(unsigned int power,
		unsigned int damage,
		unsigned int initiative,
		unsigned int currentDuration,
		UnitAttackConstParams* constParams
	);

	unsigned int power;
	unsigned int damage;
	unsigned int initiative;
	unsigned int currentDuration;

	UnitAttackConstParams* constParams;

	UnitAttack* deepCopy();
};

class Unit
{
public:
	~Unit();
	Unit(UnitConstParams* unitConstParams,
		bool isMoving,
		bool isDead,
		bool isProtected,
		bool isWaiting,
		bool isBig,
		unsigned int currentHp,
		unsigned int currentAttack,
		UnitAttack* attack1,
		UnitAttack* attack2,
		std::unordered_map<AttackClass, UnitAttack*>* attacksMap,
		unsigned int armor,
		bool retreat,
		bool paralyzed,
		bool petrified,
		bool poisoned,
		bool blistered,
		bool frostbited,
		bool damageLower,
		bool initLower,
		bool revived,
		bool damageBusted,
		bool transformed,
		unsigned int level,
		std::unordered_map<unsigned int, ImmuneCategory>* classImmune,
		std::unordered_map<unsigned int, ImmuneCategory>* sourceImmune,
		std::unordered_map<unsigned int, bool>* hasClassImmune,
		std::unordered_map<unsigned int, bool>* hasSourceImmune,
		bool isEmpty);

	Unit* deepCopy();

	UnitConstParams* unitConstParams;

	bool isMoving;
	bool isDead;
	bool isProtected;
	bool isWaiting;

	bool isBig;

	unsigned int currentHp;

	unsigned int currentAttack;

	UnitAttack* attack1;
	UnitAttack* attack2;

	std::unordered_map<AttackClass, UnitAttack*>* attacksMap;

	unsigned int armor;

	bool retreat;
	bool paralyzed;
	bool petrified;
	bool poisoned;
	bool blistered;
	bool frostbited;
	bool damageLower;
	bool initLower;
	bool revived;
	bool damageBusted;
	bool transformed;

	unsigned int level;

	std::unordered_map<unsigned int, ImmuneCategory>* classImmune;
	std::unordered_map<unsigned int, ImmuneCategory>* sourceImmune;
	std::unordered_map<unsigned int, bool>* hasClassImmune;
	std::unordered_map<unsigned int, bool>* hasSourceImmune;

	bool isEmpty;
};



enum class ImmuneCategory {
	IMMUNE_CATEGORY_NO,
	IMMUNE_CATEGORY_ONCE,
	IMMUNE_CATEGORY_ALWAYS
};

enum class AttackClass {
	/// Простой урон
	L_DAMAGE,

	/// Истощение (вампиризм)
	/// Использует урон [qty_dam] и пополняет здоровье на половину нанесённого урона
	L_DRAIN,

	/// Паралич
	L_PARALYZE,

	/// Исцеление
	L_HEAL,

	/// Страх
	L_FEAR,

	/// Увеличение урона
	/// Если [level] == 1 - +25%
	/// Если [level] == 2 - +50%
	/// Если [level] == 3 - +75%
	/// Если [level] == 4 - +100%
	L_BOOST_DAMAGE,

	/// Окаменение
	L_PETRIFY,

	/// Снижение повреждения
	/// Требует параметра игрового юнита [level]
	/// Если [level] == 1 - снижение 50%
	/// Если [level] == 2 - снижение 33%
	L_LOWER_DAMAGE,

	/// Снижение ини. Есть только один уровень, и он снижает на 50%
	L_LOWER_INITIATIVE,

	/// Яд
	L_POISON,

	/// Мороз
	L_FROSTBITE,

	/// Воскрешение
	L_REVIVE,

	/// Выпить жизненную сиду
	L_DRAIN_OVERFLOW,

	/// Лечение (эффектов)
	L_CURE,

	/// Призыв
	L_SUMMON,

	/// Понизить уровень
	L_DRAIN_LEVEL,

	/// Прибавить атаку
	L_GIVE_ATTACK,

	/// Передать жизненную сиду
	L_DOPPELGANGER,

	/// Превраить себя
	L_TRANSFORM_SELF,

	/// Превратить другого
	L_TRANSFORM_OTHER,

	/// Ожёг
	L_BLISTER,

	/// Защита от стихий
	L_BESTOW_WARDS,

	/// Разбить броню
	L_SHATTER,
};

enum class TargetsCount {
	TARGETS_COUNT_ALL,
	TARGETS_COUNT_ANY,
	TARGETS_COUNT_ONE,

	/// Ближайший юнит и две соседние цели. Урон юниту позади определяется
	/// полем атаки [DAM_RATIO]
	//oneAndTwoNearest,

	/// Ближайший юнит и одна цель за юнитом. Урон юниту позади определяется
	/// полем атаки [DAM_RATIO]
	//oneAndOneBehind,

	/// Ближайший юнит, цель рядом и цели позади за ними
	//twoFrontTwoBack
};

enum class AttackSource {
	ATTACK_SOURCE_WEAPON,
	ATTACK_SOURCE_INTELLIGENCE,
	ATTACK_SOURCE_LIFE,
	ATTACK_SOURCE_DEATH,
	ATTACK_SOURCE_FIRE,
	ATTACK_SOURCE_WATER,
	ATTACK_SOURCE_EARTH,
	ATTACK_SOURCE_AIR,
};