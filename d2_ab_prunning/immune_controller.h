#pragma once
#include <vector>
#include "unit.h"


enum class CanApplyAttackImmuneResponse;

class ImmuneController
{
public:
	CanApplyAttackImmuneResponse canApplyAttack(std::vector<Unit*>* units, int target, UnitAttack* currentAttack);

};



enum class CanApplyAttackImmuneResponse {
	CAN,
	ONCE,
	ALWAYS,
};
