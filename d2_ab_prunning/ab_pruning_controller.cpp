

#include "ab_pruning_controller.h"
#include <cassert>
#include <iostream>
#include <time.h>

ABPruningController::ABPruningController(CellUtils* cellUtils, EvaluationController* evalController)
{
    this->cellUtils = cellUtils;
    this->evalController = evalController;
}

int ABPruningController::getAction(int current, GameController* controller, int treeDepth)
{
    nodesCount = 0;
    clock_t t1, t2;
    t1 = clock();

    auto currentSnapshot = controller->deepCopy();

    // Список всех возможных действий
    int allPosibleActions[15] = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
    };

    //TODO  Нужно запомнить параметры ролла

    currentSnapshot->rollConfig->bottomTeamMaxDamage = true;
    currentSnapshot->rollConfig->bottomTeamMaxIni = true;
    currentSnapshot->rollConfig->bottomTeamMaxPower = true;

    currentSnapshot->rollConfig->topTeamMaxIni = false;
    currentSnapshot->rollConfig->topTeamMaxPower = true;
    currentSnapshot->rollConfig->topTeamMaxDamage = false;

    auto context = ABPrunningContext(allPosibleActions, true);

    std::vector<int> currentPossibleActions = std::vector<int>{};
    std::vector<int> orderedPossibleActions = std::vector<int>{};

    currentPossibleActions.reserve(15);
    // TODO Сортировка возможных действий и убрать невозможные
    // пока остаются все действия
    for (int i = 0; i < 15; i++) {
        orderedPossibleActions.push_back(allPosibleActions[i]);
    }
    // --------------------

    
    for (auto a : orderedPossibleActions) {
        auto g = currentSnapshot->deepCopy();
        auto res = g->makeAction(a);
        for (auto u : *g->units) {
             if (!u->isEmpty) delete u;
        }
        delete g;
        if (res.success) {
            currentPossibleActions.push_back(a);
        }
        if (res.endGame) {
            return a;
        }
    }

    double alpha = -INFINITY;
    double beta = INFINITY;

    int currentActiveCell = currentSnapshot->currentActiveCellIndex;

    bool isMax = cellUtils->checkIsTopTeam(currentActiveCell);
    bool isMin = !isMax;

    // TODO Проверка числа атак текущего юнита.
    // 
    // если дальность атаки - все, то обрабатывается только один клик

    std::vector<int> resultActions = std::vector<int>{};
    std::vector<double> resultValues = std::vector<double>{};

    if (isMax) {
        double maxEval = -INFINITY;
        for (auto cpa : currentPossibleActions) {
            // TODO bypass
            auto res = bypass(
                &context,
                cpa,
                currentSnapshot,
                currentActiveCell,
                alpha,
                beta,
                treeDepth,
                isMax
            );

            maxEval = res.value > maxEval ? res.value : maxEval;
            alpha = alpha > res.value ? alpha : res.value;
            if (beta <= alpha) {
                break;
            }

            resultActions.push_back(cpa);
            resultValues.push_back(res.value);
            // TODO Тут работа с юнитам с дальностью атаки - все
        }
    }
    else {
        assert(false);
    }

    int bestActionIndex = -1;
    double bestFit = -INFINITY;
    for (int i = 0; i < resultActions.size(); i++) {
        // TODO При равных прочих клик выше по приоритету
        if (resultValues[i] > bestFit) {
            bestFit = resultValues[i];
            bestActionIndex = i;
        }
    }

    t2 = clock();
    float diff = ((float)t2 - (float)t1) / CLOCKS_PER_SEC;
    std::cout << "Running time: " << diff << std::endl;
    std::cout << "Nodes: " << nodesCount << std::endl;
    std::cout << "Nodes per second: " << nodesCount / diff << std::endl;


    return resultActions[bestActionIndex];
}

double ABPruningController::calculateFitness(std::vector<Unit*>* units)
{

    return evalController->getEval(units);
}

ABPruningReturnValue ABPruningController::bypass(ABPrunningContext* context, int action, 
    GameController* snapshot, int activeCellIndex, double alpha, double beta, int treeDepth, bool isMax)
{
    context->currentTreeDepth++;
    nodesCount++;

    auto currentSnapshot = snapshot->deepCopy();
    auto res = currentSnapshot->makeAction(action);

    // TODO Оценка позиции
    double maxValue = evalController->getEval(currentSnapshot->units);

    int newActiveCellIndex = currentSnapshot->currentActiveCellIndex;

    auto newIsMax = cellUtils->checkIsTopTeam(newActiveCellIndex);

    if (res.endGame || context->currentTreeDepth > treeDepth) {
        context->currentTreeDepth--;
        return ABPruningReturnValue(maxValue);
    }

    // TODO Проверка таргетов для дальности все юниты

    std::vector<int> currentPossibleActions = std::vector<int>{};
    std::vector<int> orderedPossibleActions = std::vector<int>{};
    currentPossibleActions.reserve(15);
    orderedPossibleActions.reserve(15);

    for (int i = 0; i < 15; i++) {
        orderedPossibleActions.push_back(context->possibleActions[i]);
    }
    for (auto a : orderedPossibleActions) {
        auto g = snapshot->deepCopy();
        auto res = g->makeAction(a);
        // TODO Возможно удаление объектов запланировать на момент, когда вышли 
        // из всех уровней рекурсий
        for (auto u : *g->units) {
            if (!u->isEmpty) delete u;
        }
        delete g;
        if (res.success) {
            currentPossibleActions.push_back(a);
        }
        if (res.endGame) {
            return a;
        }
    }
    if (currentPossibleActions.size() == 0) {
        context->currentTreeDepth--;
        return ABPruningReturnValue(maxValue);
    }

    if (newIsMax) {
        double maxEval = -INFINITY;
        for (auto cpa : currentPossibleActions) {
            auto res = bypass(
                context,
                cpa,
                currentSnapshot,
                newActiveCellIndex,
                alpha,
                beta,
                treeDepth,
                newIsMax
            );

            maxEval = res.value > maxEval ? res.value : maxEval;
            alpha = alpha > res.value ? alpha : res.value;
            if (beta <= alpha) {
                break;
            }
        }
        context->currentTreeDepth--;
        return ABPruningReturnValue(maxEval);
    }
    else {
        double minEval = INFINITY;
        for (auto cpa : currentPossibleActions) {
            auto res = bypass(
                context,
                cpa,
                currentSnapshot,
                newActiveCellIndex,
                alpha,
                beta,
                treeDepth,
                newIsMax
            );

            minEval = res.value < minEval ? res.value : minEval;
            beta = beta < res.value ? beta : res.value;
            if (beta <= alpha) {
                break;
            }
        }
        context->currentTreeDepth--;
        return ABPruningReturnValue(minEval);
    }
    assert(false);
    //return ABPruningReturnValue(0.0);
}

ABPrunningContext::ABPrunningContext(int* possibleActions, bool isTopTeam)
{
    this->possibleActions = possibleActions;
    this->isTopTeam = isTopTeam;

    bestFitnessBranchNumber = -1;
    currentTreeDepth = 0;
    currentRecLevel = 0;
    alpha = 0.0;
    beta = 0.0;

}

void ABPrunningContext::addRec()
{
    currentRecLevel++;
    if (currentRecLevel > 100000000) {
        assert(false);
    }
}

ABPruningReturnValue::ABPruningReturnValue(double value)
{
    this->value = value;
    beta = 0.0;
    alpha = 0.0;
}
