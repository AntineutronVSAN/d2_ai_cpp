#include "duration_controller.h"
#include "unit.h"

DurationController::DurationController(RandomExponentialDist* random)
{
	this->random = random;
}

DurationController::~DurationController()
{
}

int DurationController::getDuration(UnitAttack* attack)
{

	if (attack->constParams->infinite) {
		return random->getNextInt(3) + 1;
	}

	return 1;
}
