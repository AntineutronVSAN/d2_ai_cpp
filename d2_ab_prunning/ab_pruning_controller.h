#pragma once

#include "game_controller.h"
#include "evaluation_controller.h"
#include "cell_utils.h"
#include <vector>

class ABPrunningContext;
class ABPruningReturnValue;
class GameController;

class ABPruningController
{
public:
	
	ABPruningController(CellUtils* cellUtils, EvaluationController* evalController);

	CellUtils* cellUtils;
	EvaluationController* evalController;

	int nodesCount = 0;

	int getAction(int current, GameController* controller, int treeDepth);

	double calculateFitness(std::vector<Unit*>* units);

private:
	ABPruningReturnValue bypass(
		ABPrunningContext* context,
		int action,
		GameController* snapshot,
		int activeCellIndex,
		double alpha,
		double beta,
		int treeDepth,
		bool isMax
	);
};

class ABPrunningContext {

public:

	ABPrunningContext(int* possibleActions, bool isTopTeam);

	int* possibleActions;
	bool isTopTeam;
	int bestFitnessBranchNumber;
	int currentTreeDepth;
	int currentRecLevel;
	double alpha;
	double beta;

	void addRec();
};

class ABPruningReturnValue {
public:
	ABPruningReturnValue(double value);
	double alpha;
	double beta;
	double value;
};