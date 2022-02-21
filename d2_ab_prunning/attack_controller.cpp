#include "attack_controller.h"
#include <cassert>
#include <iostream>


AttackController::AttackController(DurationController* durationController, DamageScatter* damageScatter, 
	PowerController* powerController, ImmuneController* immuneController, RollConfig* rollConfig, 
	CellUtils* cellUtils)
{
	this->damageScatter = damageScatter;
	this->durationController = durationController;
	this->immuneController = immuneController;
	this->powerController = powerController;
	this->rollConfig = rollConfig;
	this->cellUtils = cellUtils;

	//this->units = nullptr;
}

AttackController::~AttackController()
{
}

ApplyAttackResponse AttackController::applyAttack(int current, int target, std::vector<Unit*>* units)
{
	
	if (!units) {
		return ApplyAttackResponse::UNITS_LIST_NOT_INITIALIZED;
	}

	auto currentUnit = (*units)[current];
	auto targetUnit = (*units)[target];

	if (targetUnit->isDead || targetUnit->isEmpty) {
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
	}

	ApplyAttackContext context;
	context.current = current;
	context.target = target;
	context.botFrontLineEmpty = true;
	context.topFrontLineEmpty = true;
	context.units = units;

	for (int i = 0; i < units->size(); i++) {
		auto u = (*units)[i];
		bool hasUnit = !(u->isEmpty || u->isDead);
		context.cellHasUnit[i] = hasUnit;
		if (i >= 3 && i <= 5) {
			if (hasUnit) {
				context.topFrontLineEmpty = false;
			}
		}
		if (i >= 6 && i <= 8) {
			if (hasUnit) {
				context.botFrontLineEmpty = false;
			}
		}

	}

	auto canApply = canApplyAttack(current, target, units);
	if (canApply != ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK) {
		return canApply;
	}

	return handleAttack(context);
}

ApplyAttackResponse AttackController::handleAttack(ApplyAttackContext& context)
{
	auto units = context.units;
	auto currentUnitIndex = context.current;
	auto currentUnit = (*units)[currentUnitIndex];
	auto currentUnitAttackClass = currentUnit->attack1->constParams->attackClass;
	auto currentUnitTargetsCount = currentUnit->attack1->constParams->targetsCount;
	
	switch (currentUnitTargetsCount)
	{
	case TargetsCount::TARGETS_COUNT_ALL:
	{
		auto targetIsTopTeam = cellUtils->checkIsTopTeam(context.target);
		auto i1 = targetIsTopTeam ? 0 : 6;
		auto i2 = targetIsTopTeam ? 5 : 11;

		for (int i = 0; i < context.units->size(); i++) {
			if (i >= i1 && i <= i2) {
				if ((*(context.units))[i]->isEmpty || (*(context.units))[i]->isDead) {
					continue;
				}
				context.target = i;
				applyAttacksToUnit(context);
			}
		}
		return ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK;
	}
		break;
	case TargetsCount::TARGETS_COUNT_ANY:
		return applyAttacksToUnit(context);
	case TargetsCount::TARGETS_COUNT_ONE:
		return applyAttacksToUnit(context);
	default:
		break;
	}

	return ApplyAttackResponse();
}

ApplyAttackResponse AttackController::applyAttacksToUnit(ApplyAttackContext& context, bool handlePower)
{
	auto rollMaxPower = cellUtils->checkIsTopTeam(context.current) && rollConfig->topTeamMaxPower ||
		cellUtils->checkIsTopTeam(context.current) && rollConfig->bottomTeamMaxPower;

	auto units = context.units;
	auto currentUnit = (*units)[context.current];
	auto attack = (*units)[context.current]->attack1;
	auto attack2 = (*units)[context.current]->attack2;

	auto needHandlePower = handlePower || !rollMaxPower;

	if (!needHandlePower) {
		// TODO Immune a1 handling
		// TODO Immune a2 handling
		
		applyAttackToUnit(attack, context.current, context.target, units);
		if (!(*units)[context.target]->isDead) {
			if (attack2) {
				applyAttackToUnit(attack, context.current, context.target, context.units);
			}
		}
		
	}
	else {
		// TODO Immune a1 handling
		// TODO Immune a2 handling
		// TODO Immune a1 power
		// TODO Immune a2 power
		applyAttackToUnit(attack, context.current, context.target, units);
		if (!(*units)[context.target]->isDead) {
			if (attack2) {
				applyAttackToUnit(attack, context.current, context.target, context.units);
			}
		}
	}

	return ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK;
}

ApplyAttackResponse AttackController::canApplyAttack(int current, int target, std::vector<Unit*>* units)
{
	if (!units) {
		return ApplyAttackResponse::UNITS_LIST_NOT_INITIALIZED;
	}

	auto currentUnit = (*units)[current];
	auto targetUnit = (*units)[target];

	if (targetUnit->isDead || targetUnit->isEmpty) {
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
	}
	

	ApplyAttackContext context;
	context.current = current;
	context.target = target;
	context.botFrontLineEmpty = true;
	context.topFrontLineEmpty = true;
	context.units = units;
	//context.cellHasUnit;

	for (int i = 0; i < units->size(); i++) {
		auto u = (*units)[i];
		bool hasUnit = !(u->isEmpty || u->isDead);
		context.cellHasUnit[i] = hasUnit;
		if (i >= 3 && i <= 5) {
			if (hasUnit) {
				context.topFrontLineEmpty = false;
			}
		}
		if (i >= 6 && i <= 8) {
			if (hasUnit) {
				context.botFrontLineEmpty = false;
			}
		}
		
	}

	auto currentUnitAttack = currentUnit->attack1;
	auto currentAttackClass = currentUnitAttack->constParams->attackClass;
	auto currentAttackTargets = currentUnitAttack->constParams->targetsCount;
	
	bool currentUnitIsTopTeam = cellUtils->checkIsTopTeam(current);
	bool targetIsTopTeam = cellUtils->checkIsTopTeam(target);

	if (currentAttackTargets == TargetsCount::TARGETS_COUNT_ONE) {
		// todo подобные ONE
		if (!cellUtils->findNearestTarget(
				current, 
				target, 
				context.cellHasUnit, 
				currentUnitIsTopTeam, 
				context.topFrontLineEmpty, 
				context.botFrontLineEmpty, 0)) {

			//std::cout << "No nearest target " << std::endl;

			return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		}
	}


	switch (currentAttackClass)
	{
	case AttackClass::L_DAMAGE:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_DRAIN:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_PARALYZE:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->paralyzed || targetUnit->petrified) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_HEAL:
		if (currentUnitIsTopTeam != targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		// TODO При хиле ещё есть воскрешение

		break;
	case AttackClass::L_FEAR:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->retreat) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_BOOST_DAMAGE:
		if (currentUnitIsTopTeam != targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->damageBusted) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_PETRIFY:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->paralyzed || targetUnit->petrified) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_LOWER_DAMAGE:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->damageLower) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_LOWER_INITIATIVE:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->initLower) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_POISON:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_FROSTBITE:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_REVIVE:
		if (currentUnitIsTopTeam != targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_DRAIN_OVERFLOW:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_CURE:
		if (currentUnitIsTopTeam != targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;

		break;
	case AttackClass::L_SUMMON:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_DRAIN_LEVEL:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_GIVE_ATTACK:
		if (currentUnitIsTopTeam != targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_DOPPELGANGER:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_TRANSFORM_SELF:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_TRANSFORM_OTHER:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_BLISTER:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_BESTOW_WARDS:
		return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	case AttackClass::L_SHATTER:
		if (currentUnitIsTopTeam == targetIsTopTeam) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		if (targetUnit->isEmpty || targetUnit->isDead) return ApplyAttackResponse::UNIT_CAN_NOT_APPLY_ATTACK;
		break;
	default:
		assert(false);
	}

	return ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK;
}

ApplyAttackResponse AttackController::applyAttackToUnit(UnitAttack* attack, int current, int target, std::vector<Unit*>* units)
{
	auto targetUnit = (*units)[target];
	auto currentUnit = (*units)[current];

	bool rollMaxDamage = (cellUtils->checkIsTopTeam(current) && rollConfig->topTeamMaxDamage) ||
		(!cellUtils->checkIsTopTeam(current) && rollConfig->bottomTeamMaxDamage);

	auto currentAttackClass = attack->constParams->attackClass;

	switch (currentAttackClass)
	{
	case AttackClass::L_DAMAGE:
	{
		int currentDamage = attack->damage;
		int targetHp = targetUnit->currentHp;

		if (currentDamage <= 0) {
			// todo Случается баг, когда увеличение урона наоборот уменьшает его
			assert(currentDamage > 0);
		}

		//int damage = (damageScatter.getScattedDamage(currentDamage, rollMaxDamage: rollMaxDamage) *
		int damage = (int)((damageScatter->getScattedDamage(currentDamage, rollMaxDamage)) * getArmorRatio(targetUnit));

		int newTargetHp = targetHp - damage;

		bool isDead = false;
		if (newTargetHp <= 0) {
			newTargetHp = 0;
			isDead = true;
		}
		(*units)[target]->isDead = isDead;
		(*units)[target]->currentHp = newTargetHp; }
			break;

	case AttackClass::L_DRAIN:
	{
		assert(attack->damage > 0);

		auto currentUnit = (*units)[current];

		auto currentUnitHp = currentUnit->currentHp;
		auto currentUnitMaxHp = currentUnit->unitConstParams->maxHp;
		auto targetUnitHp = targetUnit->currentHp;

		auto currentAttackDamage = attack->damage;

		auto damage = (int)((damageScatter->getScattedDamage(currentAttackDamage, rollMaxDamage) * getArmorRatio(targetUnit)));

		auto lifeSteel = damage / 2;

		auto newTargetUnitHp = targetUnitHp - damage;

		bool targetIsDead = false;

		if (newTargetUnitHp <= 0) {
			// Если юнит умер, лайфстил идёт только на оставшуюся часть здоровья
			// цели
			lifeSteel = targetUnitHp / 2;
			newTargetUnitHp = 0;
			targetIsDead = true;
		}

		auto newCurrentUnitHp = currentUnitHp + lifeSteel;

		if (newCurrentUnitHp > currentUnitMaxHp) {
			newCurrentUnitHp = currentUnitMaxHp;
		}

		(*units)[current]->currentHp = newCurrentUnitHp;
		(*units)[target]->currentHp = newTargetUnitHp;
		(*units)[target]->isDead = targetIsDead; 
	}
		break;
	case AttackClass::L_PARALYZE:
	{
		if (targetUnit->petrified) {
			break;
		}
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);


		if (!targetUnitHasThisAttack) {

			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;

			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
			(*units)[target]->paralyzed = true;

		}

	}
		break;
	case AttackClass::L_HEAL:
	{
		if (targetUnit->isDead) {
			break;
		}

		auto currentHealVal = attack->constParams->heal;
		assert(currentHealVal > 0);
		auto targetHp = targetUnit->currentHp;
		auto maxTargetHp = targetUnit->unitConstParams->maxHp;
		auto newTargetHp = targetHp + currentHealVal;
		if (newTargetHp >= maxTargetHp) {
			newTargetHp = maxTargetHp;
		}
		(*units)[target]->currentHp = newTargetHp;
		
	}
		break;
	case AttackClass::L_FEAR:
	{
		if (!targetUnit->retreat) {
			(*units)[target]->retreat = true;
		}
	}
		break;
	case AttackClass::L_BOOST_DAMAGE:
	{
		auto attackLevel = attack->constParams->level;
		assert(attackLevel > 0 && attackLevel <= 4);

		if ((*units)[target]->attack1->constParams->firstDamage <= 0) {
			break;
		}

		auto newDamageCoeff = attackLevel * 0.25;

		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		if (!targetUnitHasThisAttack) {
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;

			(*units)[target]->damageBusted = true;
			(*units)[target]->attack1->damage += (int)((*units)[target]->attack1->constParams->firstDamage * newDamageCoeff);
		}
	}
		break;
	case AttackClass::L_PETRIFY:
	{
		if (targetUnit->paralyzed || targetUnit->petrified) {
			break;
		}
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);


		if (!targetUnitHasThisAttack) {

			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;

			(*units)[target]->petrified = true;

		}

	}
		break;
	case AttackClass::L_LOWER_DAMAGE:
	{
		auto attackLevel = attack->constParams->level;
		assert(attackLevel == 1 || attackLevel == 2);

		if ((*units)[target]->attack1->constParams->firstDamage <= 0) {
			break;
		}

		auto newDamageCoeff = attackLevel == 1 ? 0.5 : 0.33;

		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		if (!targetUnitHasThisAttack) {
			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;

			//units[target].attacksMap[attack.attackConstParams.attackClass] =
			//	attack.copyWith(currentDuration: currentAttackDuration);


			(*units)[target]->damageLower = true;
			(*units)[target]->attack1->damage -= (int)((*units)[target]->attack1->constParams->firstDamage * newDamageCoeff);
		}
	}
		break;
	case AttackClass::L_LOWER_INITIATIVE:
	{
		auto attackLevel = attack->constParams->level;
		// Судя по БД, уровень только 1
		assert(attackLevel == 1);

		auto targetUnitIniFirst = (*units)[target]->attack1->constParams->firstInitiative;
		if (targetUnitIniFirst <= 0) {
			break;
		}

		/*final targetUnitHasThisAttack =
			targetUnit.attacksMap.containsKey(attack.attackConstParams.attackClass);
		final currentAttackDuration =
			attackDurationController.getDuration(attack);*/
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		if (!targetUnitHasThisAttack) {

			/*units[target].attacksMap[attack.attackConstParams.attackClass] =
				attack.copyWith(currentDuration: currentAttackDuration);
			units[target].initLower = true;
			units[target].unitAttack.initiative = (targetUnitIniFirst ~/ 2).toInt();*/

			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;

			(*units)[target]->initLower = true;

			(*units)[target]->attack1->initiative = (targetUnitIniFirst / 2);
		}
		else {

			// Обновляем длительность, если у новой атаки она выше
			auto oldDebuffDuration =
				(*(*units)[target]->attacksMap)[attack->constParams->attackClass]->currentDuration;
			if (currentAttackDuration > oldDebuffDuration) {

				auto oldAttack = (*(*units)[target]->attacksMap)[attack->constParams->attackClass];
				(*(*units)[target]->attacksMap).erase(attack->constParams->attackClass);
				delete oldAttack;

				auto newAttack = attack->deepCopy();
				newAttack->currentDuration = currentAttackDuration;
				(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;


				/*units[target].attacksMap[attack.attackConstParams.attackClass] = attack.copyWith(
					currentDuration: currentAttackDuration,
					);*/

				(*units)[target]->initLower = true;
			}
		}
	}
		break;
	case AttackClass::L_POISON:
	{
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		assert(currentAttackDuration > 0);

		if (!targetUnitHasThisAttack) {
			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
		}
		else {
			// Если дамаг текущего яда больше, применяем новый яд
			auto currentAttackDamage =(*(*units)[target]->attacksMap)[attack->constParams->attackClass]->damage;
			auto newAttackDamage = attack->damage;
			// Если дамаг одинаковый, проверяем длительность
			auto oldUnitsAttackDuration = (*targetUnit->attacksMap)[attack->constParams->attackClass]->currentDuration;

			if (newAttackDamage > currentAttackDamage || currentAttackDuration > oldUnitsAttackDuration) {
				//units[target].attacksMap[attack.attackConstParams.attackClass] =
				//	attack.copyWith(currentDuration: currentAttackDuration);
				auto oldAttack = (*(*units)[target]->attacksMap)[attack->constParams->attackClass];
				auto newAttack = attack->deepCopy();
				newAttack->currentDuration = currentAttackDuration;
				(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
				delete oldAttack;
				break;
			}
		}

		(*units)[target]->poisoned = true;
	}
		break;
	case AttackClass::L_FROSTBITE:
	{
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		assert(currentAttackDuration > 0);

		if (!targetUnitHasThisAttack) {
			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
		}
		else {
			// Если дамаг текущего яда больше, применяем новый яд
			auto currentAttackDamage = (*(*units)[target]->attacksMap)[attack->constParams->attackClass]->damage;
			auto newAttackDamage = attack->damage;
			// Если дамаг одинаковый, проверяем длительность
			auto oldUnitsAttackDuration = (*targetUnit->attacksMap)[attack->constParams->attackClass]->currentDuration;

			if (newAttackDamage > currentAttackDamage || currentAttackDuration > oldUnitsAttackDuration) {
				//units[target].attacksMap[attack.attackConstParams.attackClass] =
				//	attack.copyWith(currentDuration: currentAttackDuration);
				auto oldAttack = (*(*units)[target]->attacksMap)[attack->constParams->attackClass];
				auto newAttack = attack->deepCopy();
				newAttack->currentDuration = currentAttackDuration;
				(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
				delete oldAttack;
				break;
			}
		}

		(*units)[target]->frostbited = true;
	}
		break;
	case AttackClass::L_REVIVE:
	{
		if (!targetUnit->isDead) {
			break;
		}

		if (targetUnit->revived) {
			break;
		}

		auto unitFirstHp = targetUnit->unitConstParams->maxHp;
		auto newHp = unitFirstHp / 2;

		(*units)[target]->isDead = false;
		(*units)[target]->currentHp = newHp;
		(*units)[target]->revived = true;

		(*units)[target]->attack1->damage = (*units)[target]->attack1->constParams->firstDamage;
		(*units)[target]->attack1->initiative = (*units)[target]->attack1->constParams->firstInitiative;

	}
		break;
	case AttackClass::L_DRAIN_OVERFLOW:
	{
		auto currentDamage = attack->damage;
		auto targetHp = targetUnit->currentHp;

		auto currentUnitHp = (*units)[current]->currentHp;
		auto currentUnitMaxHp = (*units)[current]->unitConstParams->maxHp;

		assert(currentDamage > 0);
		//auto damage = (damageScatter.getScattedDamage(currentDamage, rollMaxDamage: rollMaxDamage) *
		auto damage = (int)(damageScatter->getScattedDamage(currentDamage, rollMaxDamage) *
			getArmorRatio(targetUnit));

		auto newTargetHp = targetHp - damage;

		double lifesteel = damage / 2.0;

		auto isDead = false;
		if (newTargetHp <= 0) {
			newTargetHp = 0;
			isDead = true;
			lifesteel = targetHp / 2.0;
		}

		(*units)[target]->currentHp = newTargetHp;
		(*units)[target]->isDead = isDead;

		// Сначала лафйстилим себя, затем, если что-то осталось раздаём на остальных
		auto currentUnitDeltaHp = currentUnitMaxHp - currentUnitHp;
		if (currentUnitDeltaHp >= lifesteel) {
			// Весь лайфстил на себя
			(*units)[current]->currentHp += (int)lifesteel;
		}
		else {
			// Долечиваем себя и раздаём на остальных
			auto alliesLifesteel = lifesteel - currentUnitDeltaHp;
			assert(alliesLifesteel > 0);
			if (currentUnitDeltaHp != 0) {
				(*units)[current]->currentHp += currentUnitDeltaHp;

			}

			auto i1 = cellUtils->checkIsTopTeam(current) ? 0 : 6;
			auto i2 = cellUtils->checkIsTopTeam(current) ? 5 : 11;

			int needHeadCount = 0;

			//auto List<bool> unitNeedHeal = [];

			bool unitNeedHeal[12];

			for (int i = 0; i < units->size(); i++) {
				auto e = (*units)[i];
				if (i == current) {
					unitNeedHeal[i] = false;
					continue;
				}
				if (!(i >= i1 && i <= i2)) {
					unitNeedHeal[i] = false;
					continue;
				}
				if (e->isDead || e->isEmpty) {
					unitNeedHeal[i] = false;
					continue;
				}
				if (e->currentHp >= e->unitConstParams->maxHp) {
					unitNeedHeal[i] = false;
					continue;
				}
				unitNeedHeal[i] = true;
				needHeadCount += 1;
			}
			if (needHeadCount == 0) {
				break;
			}
			auto oneUnitHealValue = alliesLifesteel / needHeadCount;

			if (oneUnitHealValue <= 0) {
				break;
			}

			for (int i = 0; i < units->size(); i++) {
				if (unitNeedHeal[i]) {
					auto currentAllieUnitHp = (*units)[i]->currentHp;
					auto currentAllieUnitMaxHp = (*units)[i]->unitConstParams->maxHp;

					auto newHp = currentAllieUnitHp + oneUnitHealValue;
					if (newHp > currentAllieUnitMaxHp) {
						// Из лайфстила отнимается только остаток
						alliesLifesteel -= (newHp - currentAllieUnitMaxHp);

						newHp = currentAllieUnitMaxHp;
						unitNeedHeal[i] = false;
						needHeadCount -= 1;
						// Оставшийся лайфстил пересчитывается
						if (needHeadCount < 1) {
							break;
						}
						oneUnitHealValue = alliesLifesteel / needHeadCount;
					}
					else {
						alliesLifesteel -= oneUnitHealValue;
					}
					assert(!(alliesLifesteel < 0), '$alliesLifesteel');

					(*units)[i]->currentHp = newHp;
				}
			}
		}
	}
		break;
	case AttackClass::L_CURE:
	{
		if (targetUnit->attacksMap->size() == 0) {
			break;
		}

		// Снимаем все отрицательные эффекты
		//final List<AttackClass> toRemove = [];
		std::vector<AttackClass> toRemove = std::vector<AttackClass>{};
		toRemove.reserve(8);

		for (std::pair<AttackClass, UnitAttack*> targetsAtck : *(targetUnit->attacksMap)) {

			auto atckType = targetsAtck.first;
			auto atck = targetsAtck.second;

			if (atckType == AttackClass::L_PARALYZE) {
				//toRemove.add(atckType);
				toRemove.push_back(atckType);
				continue;
			}
			if (atckType == AttackClass::L_PETRIFY) {
				//toRemove.add(atckType);
				toRemove.push_back(atckType);
				continue;
			}

			// todo transformed

			if (atck->damage > 0) {
				//toRemove.add(atckType);
				toRemove.push_back(atckType);
			}

		}

		for (auto atck : toRemove) {
			auto oldAttackObj = (*(*units)[target]->attacksMap)[atck];
			(*(*units)[target]->attacksMap).erase(atck);
			delete oldAttackObj;

			//*units[target].attacksMap.remove(atck);
		}

		if (toRemove.size() > 0) {
			(*units)[target]->frostbited = false;
			(*units)[target]->blistered = false;
			(*units)[target]->paralyzed = false;
			(*units)[target]->poisoned = false;
			(*units)[target]->petrified = false;
		}
	}
		break;
	case AttackClass::L_SUMMON:
		break;
	case AttackClass::L_DRAIN_LEVEL:
		break;
	case AttackClass::L_GIVE_ATTACK:
		break;
	case AttackClass::L_DOPPELGANGER:
		break;
	case AttackClass::L_TRANSFORM_SELF:
		break;
	case AttackClass::L_TRANSFORM_OTHER:
		break;
	case AttackClass::L_BLISTER:
	{
		auto targetUnitHasThisAttack = targetUnit->attacksMap->count(attack->constParams->attackClass) == 0 ? false : true;
		auto currentAttackDuration = durationController->getDuration(attack);

		assert(currentAttackDuration > 0);

		if (!targetUnitHasThisAttack) {
			// TODO Опасный код. Выделяется память в методе deepCopy()
			// Очищение памяти запланировано в препроцессинге, когда атака спадёт
			auto newAttack = attack->deepCopy();
			newAttack->currentDuration = currentAttackDuration;
			(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
		}
		else {
			// Если дамаг текущего яда больше, применяем новый яд
			auto currentAttackDamage = (*(*units)[target]->attacksMap)[attack->constParams->attackClass]->damage;
			auto newAttackDamage = attack->damage;
			// Если дамаг одинаковый, проверяем длительность
			auto oldUnitsAttackDuration = (*targetUnit->attacksMap)[attack->constParams->attackClass]->currentDuration;

			if (newAttackDamage > currentAttackDamage || currentAttackDuration > oldUnitsAttackDuration) {
				//units[target].attacksMap[attack.attackConstParams.attackClass] =
				//	attack.copyWith(currentDuration: currentAttackDuration);
				auto oldAttack = (*(*units)[target]->attacksMap)[attack->constParams->attackClass];
				auto newAttack = attack->deepCopy();
				newAttack->currentDuration = currentAttackDuration;
				(*(*units)[target]->attacksMap)[attack->constParams->attackClass] = newAttack;
				delete oldAttack;
				break;
			}
		}

		(*units)[target]->blistered = true;
	}
		break;
	case AttackClass::L_BESTOW_WARDS:
		break;
	case AttackClass::L_SHATTER:
	{
		// todo Есть броня, которая не разбивается
		auto targetUnitArmor = targetUnit->armor;
		if (targetUnitArmor <= 0) {
			break;
		}

		auto currentAttackShakeValue = attack->damage;
		assert(currentAttackShakeValue > 0);

		if (currentAttackShakeValue > 10) {
			currentAttackShakeValue = 10;
		}

		auto newUnitArmor = targetUnitArmor - currentAttackShakeValue;
		if (newUnitArmor < 0) {
			newUnitArmor = 0;
		}

		(*units)[target]->armor = newUnitArmor;

	}
		break;
	default:
		break;
	}
	return ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK;
}

void AttackController::unitMovePostProcessing(int index, std::vector<Unit*>* units, bool waiting, bool protecting, bool retriting)
{
	
	if ((*units)[index]->isDead) {
		return;
	}

	std::vector<AttackClass> atcksToRemove = std::vector<AttackClass>{};
	atcksToRemove.reserve(8);

	for (std::pair<AttackClass, UnitAttack*> atck : *(*units)[index]->attacksMap) {
		auto atckId = atck.first;
		auto atckValue = atck.second;

		AttackClass curAtckClass = atckValue->constParams->attackClass;
		
		switch (curAtckClass)
		{
		case AttackClass::L_DAMAGE:
			break;
		case AttackClass::L_DRAIN:
			break;
		case AttackClass::L_PARALYZE:
			break;
		case AttackClass::L_HEAL:
			break;
		case AttackClass::L_FEAR:
			break;
		case AttackClass::L_BOOST_DAMAGE:
		{
			if (waiting || protecting) {
				break;
			}
			if (atckValue->currentDuration == 1) {
				auto newDamageCoeff = atckValue->constParams->level * 0.25;

				atcksToRemove.push_back(atckId);
				(*units)[index]->damageBusted = false;
				(*units)[index]->attack1->damage -= (int)((*units)[index]->attack1->constParams->firstDamage * newDamageCoeff);

			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_PETRIFY:
			break;
		case AttackClass::L_LOWER_DAMAGE:
		{
			if (waiting || protecting) {
				break;
			}
			if (atckValue->currentDuration == 1) {
				auto newDamageCoeff = atckValue->constParams->level == 1 ? 0.5 : 0.33;

				atcksToRemove.push_back(atckId);
				(*units)[index]->damageLower = false;
				(*units)[index]->attack1->damage += (int)((*units)[index]->attack1->constParams->firstDamage * newDamageCoeff);

				/*atcksToRemove.add(atckId);
				units[index].damageLower = false;
				units[index].uiInfo = 'Ослабление закончено';
				units[index].unitAttack.damage += (units[index].unitAttack.attackConstParams.firstDamage * newDamageCoeff)
					.toInt();*/

			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_LOWER_INITIATIVE:
		{
			if (atckValue->currentDuration == 1) {
				atcksToRemove.push_back(atckId);
				(*units)[index]->initLower = false;
				(*units)[index]->attack1->initiative = (*units)[index]->attack1->constParams->firstInitiative;

			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_POISON:
			break;
		case AttackClass::L_FROSTBITE:
			break;
		case AttackClass::L_REVIVE:
			break;
		case AttackClass::L_DRAIN_OVERFLOW:
			break;
		case AttackClass::L_CURE:
			break;
		case AttackClass::L_SUMMON:
			break;
		case AttackClass::L_DRAIN_LEVEL:
			break;
		case AttackClass::L_GIVE_ATTACK:
			break;
		case AttackClass::L_DOPPELGANGER:
			break;
		case AttackClass::L_TRANSFORM_SELF:
			break;
		case AttackClass::L_TRANSFORM_OTHER:
			break;
		case AttackClass::L_BLISTER:
			break;
		case AttackClass::L_BESTOW_WARDS:
			break;
		case AttackClass::L_SHATTER:
			break;
		default:
			break;
		}

		for (auto atck : atcksToRemove) {
			//units[index].attacksMap.remove(atck);
			auto oldAttack = (*(*units)[index]->attacksMap)[atck];
			(*(*units)[index]->attacksMap).erase(atck);
			delete oldAttack;
		}
	}
}

bool AttackController::unitMovePreProcessing(int index, std::vector<Unit*>* units, bool waiting, bool protecting, bool retriting)
{
	if ((*units)[index]->isDead) {
		return false;
	}

	std::vector<AttackClass> atcksToRemove = std::vector<AttackClass>{};
	atcksToRemove.reserve(8);

	auto canMove = true;

	for (std::pair<AttackClass, UnitAttack*> atck : *(*units)[index]->attacksMap) {
		auto atckId = atck.first;
		auto atckValue = atck.second;

		AttackClass curAtckClass = atckValue->constParams->attackClass;

		switch (curAtckClass)
		{
		case AttackClass::L_DAMAGE:
			break;
		case AttackClass::L_DRAIN:
			break;
		case AttackClass::L_PARALYZE:
		{
			if (atckValue->currentDuration == 1) {
				canMove = false;
				atcksToRemove.push_back(atckId);
				(*units)[index]->paralyzed = false;
			}
			else {
				canMove = false;
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_HEAL:
			break;
		case AttackClass::L_FEAR:
			break;
		case AttackClass::L_BOOST_DAMAGE:
			break;
		case AttackClass::L_PETRIFY:
		{
			if (atckValue->currentDuration == 1) {
				canMove = false;
				atcksToRemove.push_back(atckId);
				(*units)[index]->petrified = false;
			}
			else {
				canMove = false;
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_LOWER_DAMAGE:
			break;
		case AttackClass::L_LOWER_INITIATIVE:
			break;
		case AttackClass::L_POISON:
		{
			auto currentUnit = (*units)[index];
			if (currentUnit->isWaiting) {
				break;
			}
			auto poisonDamage = (*currentUnit->attacksMap)[atckId]->damage;
			auto currentUnitHp = currentUnit->currentHp;

			auto newUnitHp = currentUnitHp - poisonDamage;
			bool isDead = false;

			if (newUnitHp <= 0) {
				newUnitHp = 0;
				isDead = true;
			}

			currentUnit->isDead = isDead;
			currentUnit->currentHp = newUnitHp;

			if (atckValue->currentDuration == 1) {
				atcksToRemove.push_back(atckId);
				
				currentUnit->poisoned = false;
			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_FROSTBITE:
		{
			auto currentUnit = (*units)[index];
			if (currentUnit->isWaiting) {
				break;
			}
			auto poisonDamage = (*currentUnit->attacksMap)[atckId]->damage;
			auto currentUnitHp = currentUnit->currentHp;

			auto newUnitHp = currentUnitHp - poisonDamage;
			bool isDead = false;

			if (newUnitHp <= 0) {
				newUnitHp = 0;
				isDead = true;
			}

			currentUnit->isDead = isDead;
			currentUnit->currentHp = newUnitHp;

			if (atckValue->currentDuration == 1) {
				atcksToRemove.push_back(atckId);

				currentUnit->frostbited = false;
			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_REVIVE:
			break;
		case AttackClass::L_DRAIN_OVERFLOW:
			break;
		case AttackClass::L_CURE:
			break;
		case AttackClass::L_SUMMON:
			break;
		case AttackClass::L_DRAIN_LEVEL:
			break;
		case AttackClass::L_GIVE_ATTACK:
			break;
		case AttackClass::L_DOPPELGANGER:
			break;
		case AttackClass::L_TRANSFORM_SELF:
			break;
		case AttackClass::L_TRANSFORM_OTHER:
			break;
		case AttackClass::L_BLISTER:
		{
			auto currentUnit = (*units)[index];
			if (currentUnit->isWaiting) {
				break;
			}
			auto poisonDamage = (*currentUnit->attacksMap)[atckId]->damage;
			auto currentUnitHp = currentUnit->currentHp;

			auto newUnitHp = currentUnitHp - poisonDamage;
			bool isDead = false;

			if (newUnitHp <= 0) {
				newUnitHp = 0;
				isDead = true;
			}

			currentUnit->isDead = isDead;
			currentUnit->currentHp = newUnitHp;

			if (atckValue->currentDuration == 1) {
				atcksToRemove.push_back(atckId);

				currentUnit->blistered = false;
			}
			else {
				(*(*units)[index]->attacksMap)[atckId]->currentDuration -= 1;
			}
		}
			break;
		case AttackClass::L_BESTOW_WARDS:
			break;
		case AttackClass::L_SHATTER:
			break;
		default:
			break;
		}

		for (auto atck : atcksToRemove) {
			auto oldAttack = (*(*units)[index]->attacksMap)[atck];
			(*(*units)[index]->attacksMap).erase(atck);
			delete oldAttack;
		}
	}
	return canMove;
}

double AttackController::getArmorRatio(Unit* unit)
{

	double armorRatio = 1.0 - unit->armor / 100.0;
	armorRatio /= (unit->isProtected ? 2.0 : 1.0);
	if (armorRatio < 0.1) {
		armorRatio = 0.1;
	}
	return armorRatio;
}


