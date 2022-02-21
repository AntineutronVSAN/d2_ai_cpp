#pragma once
#include "random_exponential_dist.h"

const int MAX_ROLL_DAMAGE_VALUE_CONST = 9;

class DamageScatter
{
public:

	DamageScatter(RandomExponentialDist* random);
	~DamageScatter();

	int getScattedDamage(int damage, bool rollMaxDamage);

	RandomExponentialDist* random;
};

