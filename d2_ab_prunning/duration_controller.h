#pragma once
#include "random_exponential_dist.h"
#include "unit.h"

class DurationController
{
public:
	DurationController(RandomExponentialDist* random);
	~DurationController();

	int getDuration(UnitAttack* attack);

	RandomExponentialDist* random;
};

