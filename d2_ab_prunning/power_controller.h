#pragma once
#include "unit.h"
#include "random_exponential_dist.h"

class PowerController
{
public:

	PowerController(RandomExponentialDist* random);
	~PowerController();

	RandomExponentialDist* random;

	bool applyAttack(UnitAttack* attack, bool rollMaxPower=false);


};

