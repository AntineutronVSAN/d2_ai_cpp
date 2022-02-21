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
	/// ������� ����
	L_DAMAGE,

	/// ��������� (���������)
	/// ���������� ���� [qty_dam] � ��������� �������� �� �������� ���������� �����
	L_DRAIN,

	/// �������
	L_PARALYZE,

	/// ���������
	L_HEAL,

	/// �����
	L_FEAR,

	/// ���������� �����
	/// ���� [level] == 1 - +25%
	/// ���� [level] == 2 - +50%
	/// ���� [level] == 3 - +75%
	/// ���� [level] == 4 - +100%
	L_BOOST_DAMAGE,

	/// ����������
	L_PETRIFY,

	/// �������� �����������
	/// ������� ��������� �������� ����� [level]
	/// ���� [level] == 1 - �������� 50%
	/// ���� [level] == 2 - �������� 33%
	L_LOWER_DAMAGE,

	/// �������� ���. ���� ������ ���� �������, � �� ������� �� 50%
	L_LOWER_INITIATIVE,

	/// ��
	L_POISON,

	/// �����
	L_FROSTBITE,

	/// �����������
	L_REVIVE,

	/// ������ ��������� ����
	L_DRAIN_OVERFLOW,

	/// ������� (��������)
	L_CURE,

	/// ������
	L_SUMMON,

	/// �������� �������
	L_DRAIN_LEVEL,

	/// ��������� �����
	L_GIVE_ATTACK,

	/// �������� ��������� ����
	L_DOPPELGANGER,

	/// ��������� ����
	L_TRANSFORM_SELF,

	/// ���������� �������
	L_TRANSFORM_OTHER,

	/// ���
	L_BLISTER,

	/// ������ �� ������
	L_BESTOW_WARDS,

	/// ������� �����
	L_SHATTER,
};

enum class TargetsCount {
	TARGETS_COUNT_ALL,
	TARGETS_COUNT_ANY,
	TARGETS_COUNT_ONE,

	/// ��������� ���� � ��� �������� ����. ���� ����� ������ ������������
	/// ����� ����� [DAM_RATIO]
	//oneAndTwoNearest,

	/// ��������� ���� � ���� ���� �� ������. ���� ����� ������ ������������
	/// ����� ����� [DAM_RATIO]
	//oneAndOneBehind,

	/// ��������� ����, ���� ����� � ���� ������ �� ����
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