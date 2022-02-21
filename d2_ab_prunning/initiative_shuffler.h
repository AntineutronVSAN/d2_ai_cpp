#pragma once

#include <vector>
#include "unit.h"
#include "roll_config.h"
#include "random_exponential_dist.h"

const int MAX_INITIATIVE_ROLL = 9;

class InitiativeShuffler
{

public:

	InitiativeShuffler(RandomExponentialDist* random);

	RandomExponentialDist* random;

	void shuffleAndSort(std::vector<Unit*>* unitsRef, RollConfig* rollConfig);

};

