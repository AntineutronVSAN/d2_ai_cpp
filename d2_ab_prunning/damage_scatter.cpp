#include "damage_scatter.h"

DamageScatter::DamageScatter(RandomExponentialDist* random)
{
	this->random = random;
}

DamageScatter::~DamageScatter()
{
}

int DamageScatter::getScattedDamage(int damage, bool rollMaxDamage)
{
	if (rollMaxDamage) {
		return damage + MAX_ROLL_DAMAGE_VALUE_CONST;
	}
	return damage + random->getNextInt(MAX_ROLL_DAMAGE_VALUE_CONST);
}
