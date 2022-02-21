#include "power_controller.h"

PowerController::PowerController(RandomExponentialDist* random)
{
    this->random = random;
}

PowerController::~PowerController()
{
}

bool PowerController::applyAttack(UnitAttack* attack, bool rollMaxPower)
{

    if (rollMaxPower) {
        return true;
    }

    auto attackPower = attack->power;

    if (attackPower >= 100) {
        return true;
    }
    if (attackPower <= 0) {
        return false;
    }
    auto nextRandomNum = random->getNextInt(100);

    return attackPower > nextRandomNum;
}
