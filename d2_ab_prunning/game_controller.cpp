
#include "game_controller.h"
#include <cassert>
#include "units_factory.h"

GameController::GameController(ABPruningController* aiController, AttackController* controller, RollConfig* rollConfig, InitiativeShuffler* initiativeShuffler, Unit* emptyUnit)
{
	currentActiveCellIndex = -1;
	currentRound = -1;
	gameStarted = false;
	unitPosition = new std::unordered_map<std::string, int>;
	inited = false;
	unitsRef = new std::vector<Unit*>;
	units = new std::vector<Unit*>;
	unitsQueue = new std::deque<Unit*>;
	endGame = false;

	this->rollConfig = rollConfig;
	this->initiativeShuffler = initiativeShuffler;
	this->controller = controller;
	this->aiController = aiController;

	//emptyUnit = new Unit();
	this->emptyUnit = emptyUnit;
	//emptyUnit->isEmpty = true;
	//emptyUnit->unitConstParams.unitWarId = EMPTY_UNIT_IDS;
	//emptyUnit->unitConstParams.unitGameId = EMPTY_UNIT_IDS;
}

GameController::~GameController()
{

	delete units;
	delete unitPosition;
	delete unitsRef;
	delete unitsQueue;
	//delete emptyUnit;
}

ResponseAction GameController::init(std::vector<Unit*>* units)
{
	for (int i = 0; i < CELLS_COUNT; i++) {
		this->units->push_back((*units)[i]);
		unitsRef->push_back((*units)[i]);
		(*unitPosition)[(*units)[i]->unitConstParams->unitWarId] = i;

	}

	ResponseAction response = ResponseAction();
	response.endGame = false;
	response.success = true;
	response.currentActiveCell = -1;

	this->inited = true;

	return response;
}

ResponseAction GameController::startGame()
{

	if (!inited || endGame || gameStarted) {
		return _getErrorResponse();
	}

	auto result = _startNewRound();

	if (!result) {
		return _getErrorResponse();
	}

	auto currentActiveUnit = unitsQueue->front();
	unitsQueue->pop_front();

	currentActiveCellIndex = (*unitPosition)[currentActiveUnit->unitConstParams->unitWarId];

	for (int i = 0; i < 12; i++) {
		if (i == currentActiveCellIndex) {
			(*units)[i]->isMoving = true;
		}
		else {
			(*units)[i]->isMoving = false;
		}
	}

	return _getSuccessResponse();
}

ResponseAction GameController::makeAction(int action)
{
	if (!_hasAliveUnits()) {
		endGame = true;
		return _getEndGameResponse();
	}

	switch (action)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		return _handleClick(action);
	case ACTION_PROTECT_CODE:
		return _handleProtect();
	case ACTION_WAIT_CODE:
		return _handleWait();
	case ACTION_RETREAT_CODE:
		return _handleRetreat();
	default:
		//assert(false);
		return _getErrorResponse();
	}
	//assert(false);
	return _getErrorResponse();
}

void GameController::reset()
{

	this->currentActiveCellIndex = -1;
	this->currentRound = -1;
	this->gameStarted = false;
	this->unitPosition->clear();
	this->inited = false;
	this->unitsRef->clear();
	this->units->clear();
	//this->unitsQueue; // TODO
	this->endGame = false;
}

Unit* GameController::getEmptyUnit()
{
	return emptyUnit;
}

GameController* GameController::deepCopy()
{
	auto copy = new GameController(
		this->aiController,
		this->controller,
		this->rollConfig,
		this->initiativeShuffler,
		this->emptyUnit);

	auto copyUnits = new std::vector<Unit*>{};
	auto copyUnitsRef = new std::vector<Unit*>{};
	auto copyUnitPosition = new std::unordered_map<std::string, int>;
	auto copyUnitsQueue = new std::deque<Unit*>;

	auto newUnitsHash = std::unordered_map<std::string, Unit*>{};

	for (auto u : *units) {
		auto copyUnit = u->deepCopy();
		copyUnits->push_back(copyUnit);
		newUnitsHash[copyUnit->unitConstParams->unitWarId] = copyUnit;
	}
	for (auto pair : *unitPosition) {
		(*copyUnitPosition)[pair.first] = pair.second;
	}
	for (auto oldRef : *unitsRef) {
		auto oldUnitWarId = oldRef->unitConstParams->unitWarId;
		copyUnitsRef->push_back(newUnitsHash[oldUnitWarId]);
	}
	for (auto u : *unitsQueue) {
		auto oldUnitWarId = u->unitConstParams->unitWarId;
		copyUnitsQueue->push_back(newUnitsHash[oldUnitWarId]);
	}
	copy->units = copyUnits;
	copy->unitsRef = copyUnitsRef;
	copy->unitPosition = copyUnitPosition;
	copy->unitsQueue = copyUnitsQueue;
	copy->inited = inited;
	copy->gameStarted = gameStarted;
	copy->endGame = endGame;
	copy->currentActiveCellIndex = currentActiveCellIndex;
	copy->currentRound = currentRound;

	return copy;
}

ResponseAction GameController::_getSuccessResponse()
{
	auto response = ResponseAction();
	response.success = true;
	response.currentActiveCell = currentActiveCellIndex;
	response.endGame = this->endGame;

	return response;
}

ResponseAction GameController::_getErrorResponse()
{
	auto response = ResponseAction();
	response.success = false;
	response.currentActiveCell = currentActiveCellIndex;
	response.endGame = this->endGame;

	return response;
}

ResponseAction GameController::_getEndGameResponse()
{
	auto response = ResponseAction();
	response.success = false;
	response.currentActiveCell = -1;
	assert(endGame);
	response.endGame = endGame;

	return response;
}

ResponseAction GameController::_handleClick(int target)
{

	// TODO Применяем атаку контроллером
	
	auto applyResult = controller->applyAttack(currentActiveCellIndex, target, units);
	if (applyResult != ApplyAttackResponse::UNIT_CAN_APPLY_ATTACK) {
		return _getErrorResponse();
	}

	// TODO Проверяем, что атака применилась куорректно

	// Снимается ожидание после удачного клика
	(*units)[currentActiveCellIndex]->isWaiting = false;

	return _getNextUnit(target, true, false, false, false);
}

ResponseAction GameController::_handleWait()
{
	auto currentUnit = (*units)[currentActiveCellIndex];
	if (currentUnit->currentAttack == 1 || currentUnit->isWaiting) {
		return _getErrorResponse();
	}

	currentUnit->isWaiting = true;
	unitsQueue->push_back(currentUnit);

	return _getNextUnit(ACTION_WAIT_CODE, false, true, false, false);
}

ResponseAction GameController::_handleProtect()
{
	auto currentUnit = (*units)[currentActiveCellIndex];

	//assert(!currentUnit->isProtected);
	currentUnit->isProtected = true;
	// Если ждал, ожидание снимается
	currentUnit->isWaiting = false;
	return _getNextUnit(ACTION_PROTECT_CODE, false, false, true, false);
}

ResponseAction GameController::_handleRetreat()
{
	auto currentUnit = (*units)[currentActiveCellIndex];

	currentUnit->retreat = true;
	currentUnit->isWaiting = false;
	currentUnit->isProtected = false;

	return _getNextUnit(ACTION_RETREAT_CODE, false, false, false, true);

}

ResponseAction GameController::_getNextUnit(int action,
	bool handleDoubleAttack,
	bool waiting,
	bool protecting,
	bool retriting)
{
	auto currentUnit = (*units)[currentActiveCellIndex];

	// Проверка двойной атаки
	if (handleDoubleAttack && currentUnit->unitConstParams->isDoubleAttack) {
		if (currentUnit->currentAttack == 0) {
			currentUnit->currentAttack = 1;
			return _getSuccessResponse();
		}
		currentUnit->currentAttack = 0;
	}

	// TODO Контроллер атак - построцессинг
	controller->unitMovePostProcessing(currentActiveCellIndex, units, waiting, protecting, retriting);

	while (true) {
		// Стартуется новый раунд, если не успешно считаем, что игра окончена
		if (unitsQueue->size() == 0) {
			if (!_startNewRound()) {
				return _getEndGameResponse();
			}
		}

		auto currentUnit = unitsQueue->front();
		auto currentActiveUnitId = currentUnit->unitConstParams->unitWarId;
		
		if (currentUnit->isEmpty) {
			continue;
		}
		auto currentActiveUnitCell = (*unitPosition)[currentActiveUnitId];

		unitsQueue->pop_front();
		currentUnit = (*units)[currentActiveUnitCell];

		// TODO Препроцессинг контроллером атак
		// Возвращает false, если юнит не может ходить
		bool preprocessingResult = controller->unitMovePreProcessing(currentActiveUnitCell, units, waiting, protecting, retriting);

		if (!preprocessingResult) {
			currentUnit->isProtected = false;
			currentUnit->isWaiting = false;
			if (currentUnit->isDead) {
				// TODO
			}
			continue;
		}

		// Юнит мог умереть после препроцессинга
		if (currentUnit->isDead) {
			continue;
		}

		if (currentUnit->retreat) {
			auto emptyUnit = getEmptyUnit();

			auto retreatedUnitWarId = (*units)[currentActiveUnitCell]->unitConstParams->unitWarId;
			
			// TODO Этот колхоз можно оптимизировать
			int retreatedUnitRefIndex = -1;
			for (int i = 0; i < unitsRef->size(); i++) {
				if ((*unitsRef)[i]->unitConstParams->unitWarId == retreatedUnitWarId) {
					retreatedUnitRefIndex = i;
					break;
				}
			}
			assert(retreatedUnitRefIndex != -1);
			(*units)[currentActiveUnitCell] = emptyUnit;
			(*unitsRef)[retreatedUnitRefIndex] = emptyUnit;
			// ХЗ как, но тут удалось из очереди по индексу

		}

		this->currentActiveCellIndex = currentActiveUnitCell;

		currentUnit->isProtected = false;

		_setMoveUnitByIndex(currentActiveUnitCell);

		break;
	}

	

	return _getSuccessResponse();
}

void GameController::_setMoveUnitByIndex(int index)
{
	assert(index == currentActiveCellIndex); // TODO Убрать потом
	for (int i = 0; i < 12; i++) {
		if (i == index) {
			(*units)[i]->isMoving = true;
		}
		else {
			(*units)[i]->isMoving = false;
		}
	}
}

bool GameController::_hasAliveUnits()
{
	bool topTeamHasAlive = false;
	bool botTeamHasAlive = false;

	for (int i = 0; i < units->size(); i++) {
		auto currentUnit = (*units)[i];
		if (i <= 5) {
			if (!currentUnit->isDead & !currentUnit->isEmpty) {
				topTeamHasAlive = true;
			}
		}
		else if ((i > 5) & (i <= 11)) {
			if (!currentUnit->isDead & !currentUnit->isEmpty) {
				botTeamHasAlive = true;
			}
		}
	}

	endGame = !(topTeamHasAlive && botTeamHasAlive);

	return topTeamHasAlive && botTeamHasAlive;
}

bool GameController::_startNewRound()
{
	this->unitsRef->clear();

	for (Unit* u : *(this->units)) {
		if (u->isDead || u->isEmpty) {
			continue;
		}
		this->unitsRef->push_back(u);
	}
	if (this->unitsRef->size() == 0) {
		return false;
	}  
	
	_sortUnitsByInitiative();

	if (unitsQueue->size() == 0) {
		return false;
	}

	this->currentRound++;

	return true;
}

void GameController::_sortUnitsByInitiative()
{
	this->initiativeShuffler->shuffleAndSort(this->unitsRef, this->rollConfig);
	if (unitsQueue->size() != 0) {
		assert(false);
	}
	for (auto u : *(this->unitsRef)) {
		unitsQueue->push_back(u);
	}
}
