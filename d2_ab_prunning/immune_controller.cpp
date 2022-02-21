#include "immune_controller.h"

CanApplyAttackImmuneResponse ImmuneController::canApplyAttack(std::vector<Unit*>* units, int target, UnitAttack* currentAttack)
{
    return CanApplyAttackImmuneResponse::CAN;
}
