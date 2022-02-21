#include "evaluation_controller.h"

double EvaluationController::getEval(std::vector<Unit*>* units)
{
    // TODO Доработать
    // Пока это тупо хп

    int aiTeamHp = 0;
    int enemyTeamHp = 0;

    int aiTeamMaxHp = 0;
    int enemyTeamMaxHp = 0;

    int index = 0;
    for (auto u : *units) {

        if (index <= 5) {
            // TOP
            aiTeamHp += u->currentHp;
            aiTeamMaxHp += u->unitConstParams->maxHp;
        }
        else {
            // BOT
            enemyTeamHp += u->currentHp;
            enemyTeamMaxHp += u->unitConstParams->maxHp;
        }

        index++;
    }

    double oursEval = (double)aiTeamHp / (double)aiTeamMaxHp;
    double enemyEval = 1.0 - (double)enemyTeamHp / (double)enemyTeamMaxHp;

    return oursEval*0.5 + enemyEval;
}
