#pragma once

#include <unordered_map>
#include <deque>
#include <memory>

#include "unit.h"
#include "roll_config.h"
#include "initiative_shuffler.h"
#include "attack_controller.h"
#include "ab_pruning_controller.h"

/*

Описание совершаемых действий.
0 - Клик на 0
1 - Клик на 1
...
11 - Клик на 11

12 - Защита
13 - Ждать
14 - Отступить

*/

const int CELLS_COUNT = 12;

const int ACTION_PROTECT_CODE = 12;
const int ACTION_WAIT_CODE = 13;
const int ACTION_RETREAT_CODE = 14;

struct ResponseAction;
class ABPruningController;

class GameController
{
public:

	GameController(ABPruningController* aiController, AttackController* controller, RollConfig* rollConfig, InitiativeShuffler* initiativeShuffler, Unit* emptyUnit);
	~GameController();

	RollConfig* rollConfig;
	InitiativeShuffler* initiativeShuffler;
	AttackController* controller;
	ABPruningController* aiController;

	Unit* emptyUnit;

	std::vector<Unit*>* units;
	std::vector<Unit*>* unitsRef;
	std::unordered_map<std::string, int>* unitPosition;
	std::deque<Unit*>* unitsQueue;

	bool inited;
	bool gameStarted;
	bool endGame;

	int currentActiveCellIndex;
	int currentRound;

	ResponseAction init(std::vector<Unit*>* units);

	ResponseAction startGame();

	ResponseAction makeAction(int action);

	void reset();

	Unit* getEmptyUnit();

	GameController* deepCopy();

private:

	ResponseAction _getSuccessResponse();
	ResponseAction _getErrorResponse();
	ResponseAction _getEndGameResponse();

	ResponseAction _handleClick(int target);
	ResponseAction _handleWait();
	ResponseAction _handleProtect();
	ResponseAction _handleRetreat();

	ResponseAction _getNextUnit(
		int action, 
		bool handleDoubleAttack = false,
		bool waiting = false,
		bool protecting = false,
		bool retriting = false
		);

	void _setMoveUnitByIndex(int index);

	bool _hasAliveUnits();

	bool _startNewRound();
	void _sortUnitsByInitiative();
};


struct ResponseAction
{
	bool endGame;
	bool success;
	int16_t currentActiveCell;
	// int16_t reason // Причина ошибки
};