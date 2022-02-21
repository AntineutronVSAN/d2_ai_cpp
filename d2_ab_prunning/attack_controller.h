#pragma once

#include "duration_controller.h"
#include "damage_scatter.h"
#include "power_controller.h"
#include "immune_controller.h"
#include "roll_config.h"
#include "cell_utils.h"

enum class ApplyAttackResponse;
struct ApplyAttackContext;

class AttackController
{

public:

	AttackController(DurationController* durationController,
		DamageScatter* damageScatter,
		PowerController* powerController,
		ImmuneController* immuneController, 
		RollConfig* rollConfig, CellUtils* cellUtils);

	~AttackController();

	DurationController* durationController;
	DamageScatter* damageScatter;
	PowerController* powerController;
	ImmuneController* immuneController;
	RollConfig* rollConfig;
	CellUtils* cellUtils;

	ApplyAttackResponse applyAttack(int current, int target, std::vector<Unit*>* units);

	void unitMovePostProcessing(
		int index,
		std::vector<Unit*>* units,
		bool waiting,
		bool protecting,
		bool retriting
	);
	bool unitMovePreProcessing(
		int index,
		std::vector<Unit*>* units,
		bool waiting,
		bool protecting,
		bool retriting
	);

private:
	//std::vector<Unit*>* units;

	ApplyAttackResponse handleAttack(
		ApplyAttackContext& context
	);

	ApplyAttackResponse applyAttacksToUnit(
		ApplyAttackContext& context,
		bool handlePower=true
	);

	/// <summary>
	/// Может ли юнит применить свою атаку
	/// </summary>
	/// <param name="current">Кто применяет</param>
	/// <param name="target">На кого</param>
	/// <returns></returns>
	ApplyAttackResponse canApplyAttack(int current, int target, std::vector<Unit*>* units);

	/// <summary>
	/// Применяет атаку к юниту. Перед вызовом этого метода
	/// всё должно быть проверено.
	/// Метод ничего не знает о числе таргетов.
	/// </summary>
	/// <returns></returns>
	ApplyAttackResponse applyAttackToUnit(UnitAttack* attack, int current, int target, std::vector<Unit*>* units);

	

	double getArmorRatio(Unit* unit);
};


enum class ApplyAttackResponse {
	UNIT_CAN_APPLY_ATTACK,
	UNITS_LIST_NOT_INITIALIZED,
	UNIT_CAN_NOT_APPLY_ATTACK,
};


struct ApplyAttackContext
{
	int current;
	int target;
	std::vector<Unit*>* units;
	bool cellHasUnit[12];
	bool topFrontLineEmpty;
	bool botFrontLineEmpty;

};