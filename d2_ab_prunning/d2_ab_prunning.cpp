
#include <iostream>

#include "game_controller.h"
#include "power_controller.h"
#include "duration_controller.h"
#include "damage_scatter.h"
#include "immune_controller.h"
#include "attack_controller.h"
#include <cassert>
#include "evaluation_controller.h"
//#include "ab_pruning_controller.h"

Unit* getEmptyUnit();
Unit* getSimpleUnit(int number);

void printResponse(ResponseAction resp);
void printUnits(std::vector<Unit*>* units);

void handleAiMove(ABPruningController* ai, GameController* controller, int activeCell);

typedef std::string(*CallbackType)(Unit* u, int index);

const int TREE_DEPTH_CONST = 5;


int main()
{
    // ------------ Внедрение зависимостей -----------------
    RandomExponentialDist* random = new RandomExponentialDist();
    RollConfig* rollConfig = new RollConfig();

    InitiativeShuffler* initiativeShuffler = new InitiativeShuffler(random);
    PowerController* powerController = new PowerController(random);
    DurationController* durationController = new DurationController(random);
    DamageScatter* damageScatter = new DamageScatter(random);

    ImmuneController* immuneController = new ImmuneController();

    CellUtils* cellUtils = new CellUtils();

    AttackController* attackController = new AttackController(durationController, damageScatter, powerController, immuneController, rollConfig, cellUtils);

    EvaluationController* evaluationController = new EvaluationController();

    ABPruningController* aBPruningController = new ABPruningController(cellUtils, evaluationController);
    // -----------------------------------------------------

    // -------------- Глобальный пустой юнит
    auto globalEmptyUnit = getEmptyUnit();
    // -----------------------------------------------------

    GameController controller = GameController(aBPruningController, attackController, rollConfig, initiativeShuffler, globalEmptyUnit);

    auto units = new std::vector<Unit*>{};
    units->reserve(12);
    

    for (int i = 0; i < 12; i++) {
        if ((i + 1) % 2 == 0) {
            units->push_back(globalEmptyUnit);
            //units->push_back(getSimpleUnit(i));
        }
        else {
            units->push_back(getSimpleUnit(i));
        }
        
    }

    auto test = (*units)[0]->deepCopy();
    auto test2 = (*units)[1]->deepCopy();

    auto controllersUnits = controller.units;

    //std::cout << "Unit 1. Address: " << (*controllersUnits)[0]. << std::endl;

    ////std::cout << unit.armor << std::endl;
    ////std::cout << unit.blistered << std::endl;
    controller.init(units);
    auto result = controller.startGame();
    printResponse(result);

    while (true)
    {
        int action;
        printUnits(controllersUnits);

        std::cin >> action;
        system("CLS");
        if (action == 255) {
            break;
        }
        result = controller.makeAction(action);
        printResponse(result);
        
        // Если ходит ИИ, передаём ход ему
        if (result.currentActiveCell <= 5) {
            handleAiMove(aBPruningController, &controller, result.currentActiveCell);
        }

    }
}



void handleAiMove(ABPruningController* ai, GameController* controller, int activeCell) {
    auto res = ai->getAction(activeCell, controller, TREE_DEPTH_CONST);
    auto result = controller->makeAction(res);

    if (!result.success) {
        if (result.endGame) {
            return;
        }
    }

    printResponse(result);
    printUnits(controller->units);
    //system("CLS");



    if (result.currentActiveCell <= 5) {
        handleAiMove(ai, controller, result.currentActiveCell);
    }

}

void printUnits(std::vector<Unit*>* units) {

    std::unordered_map<int, int> index2line = std::unordered_map<int, int>({
        {0,0},
        {1,0},
        {2,0},
        {3,1},
        {4,1},
        {5,1},
        {6,2},
        {7,2},
        {8,2},
        {9,3},
        {10,3},
        {11,3},
    });

    std::vector<CallbackType> unitItems = std::vector<CallbackType>{

        [](Unit* u, int index) {
            
            return "Cell: " + std::to_string(index);
        },
        [](Unit* u, int index) {
            if (u->isEmpty || u->isDead) {
                std::string res = "";
                return res;
            }
            auto isMov = u->isMoving;
            std::string result;
            if (isMov) {
                result = "---->>>> Move: ";
            }
            else {
                result = "Move: ";
            }
            return result + std::to_string(isMov);
        },
        [](Unit* u, int index) {
           if (u->isEmpty || u->isDead) {
                std::string res = "";
                return res;
            }
            return "Dead: " + std::to_string(u->isDead);
        },
        [](Unit* u, int index) {
           if (u->isEmpty || u->isDead) {
                std::string res = "";
                return res;
            }
            return "Hp: " + std::to_string(u->currentHp);
        },
        [](Unit* u, int index) {
           if (u->isEmpty || u->isDead) {
                std::string res = "";
                return res;
            }
            return "Damage: " + std::to_string(u->attack1->damage);
        },
            [](Unit* u, int index) {
           if (u->isEmpty || u->isDead) {
                std::string res = "";
                return res;
            }
            return "Waiting: " + std::to_string(u->isWaiting);
        },
    };

    std::unordered_map<int, std::vector<std::string>> linesContent = std::unordered_map<int, std::vector<std::string>>{};

    linesContent[0] = std::vector<std::string>{ };
    linesContent[1] = std::vector<std::string>{ };
    linesContent[2] = std::vector<std::string>{ };
    linesContent[3] = std::vector<std::string>{ };
    for (auto _ : unitItems) {
        linesContent[0].push_back("");
        linesContent[1].push_back("");
        linesContent[2].push_back("");
        linesContent[3].push_back("");
    }

    int lastLine = -1;

    const int fixedEmptySize = 50;
    const int fixedUnitSize = 10;

    for (int i = 0; i < units->size(); i++) {
        auto curUnit = (*units)[i];
        auto lineNumber = index2line[i];
        if (lineNumber != lastLine) {
            if (lastLine >= 0) {
                for (int j = 0; j < unitItems.size(); j++) {
                    linesContent[lastLine][j] += "\n";
                }
            }
            
            lastLine = lineNumber;
        }

        for (int j = 0; j < unitItems.size(); j++) {
            auto curItem = unitItems[j];

            auto newVal = curItem(curUnit, i);
            auto newValLen = newVal.length();
            auto delta = fixedEmptySize - newValLen;
            assert(delta > 0);
            linesContent[lineNumber][j] += std::string(delta, ' ');
            linesContent[lineNumber][j] += newVal;
        }

    }
    for (int j = 0; j < unitItems.size(); j++) {
        linesContent[lastLine][j] += "\n";
    }

    for (auto val : linesContent[0]) {
        for (auto line : val) {
            std::cout << line;
        }
    }
    std::cout << "\n";
    for (auto val : linesContent[1]) {
        for (auto line : val) {
            std::cout << line;
        }
    }
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";
    for (auto val : linesContent[2]) {
        for (auto line : val) {
            std::cout << line;
        }
    }
    std::cout << "\n";
    for (auto val : linesContent[3]) {
        for (auto line : val) {
            std::cout << line;
        }
    }
    std::cout << "\n";
}

void printResponse(ResponseAction resp) {
    std::cout << "\n---------- RESPONSE ACTION START ------------" << std::endl;
    std::cout << "End game? " << resp.endGame << std::endl;
    std::cout << "Who move " << resp.currentActiveCell << std::endl;
    std::cout << "Action success? " << resp.success << std::endl;
    std::cout << "---------- RESPONSE ACTION END ------------\n" << std::endl;
}

Unit* getEmptyUnit() {

    auto unitConstParams = new UnitConstParams(
        100,
        "Пустышка",
        "EMMPY",
        "EMMPY",
        false,
        5,
        5,
        5,
        5,
        5,
        5,
        false);

    auto unitAttackConstParams = new UnitAttackConstParams(
        0,
        50,
        50,
        TargetsCount::TARGETS_COUNT_ONE,
        AttackClass::L_DAMAGE,
        false,
        "ATCK_ID",
        0,
        AttackSource::ATTACK_SOURCE_WEAPON
    );

    auto unitAttack1 = new UnitAttack(
        80,
        50,
        50,
        0,
        unitAttackConstParams
    );

    auto attacksMap = new std::unordered_map<AttackClass, UnitAttack*>;
    auto classImmune = new std::unordered_map<unsigned int, ImmuneCategory>;
    auto sourceImmune = new std::unordered_map<unsigned int, ImmuneCategory>;
    auto hasClassImmune = new std::unordered_map<unsigned int, bool>;
    auto hasSourceImmune = new std::unordered_map<unsigned int, bool>;

    auto emptyUnit = new Unit(
        unitConstParams,
        false,
        false,
        false,
        false,
        false,
        100,
        0,
        unitAttack1,
        nullptr,
        attacksMap,
        0,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        1,
        classImmune,
        sourceImmune,
        hasClassImmune,
        hasSourceImmune,
        true
    );

    return emptyUnit;
}

Unit* getSimpleUnit(int number) {

    auto unitConstParams = new UnitConstParams(
        10000,
        "Пустышка",
        std::to_string(number),
        std::to_string(number),
        false,
        5,
        5,
        5,
        5,
        5,
        5,
        false);

    auto unitAttackConstParams = new UnitAttackConstParams(
        0,
        50,
        50,
        TargetsCount::TARGETS_COUNT_ANY,
        AttackClass::L_DAMAGE,
        false,
        "ATCK_ID",
        0,
        AttackSource::ATTACK_SOURCE_WEAPON
    );

    auto unitAttack1 = new UnitAttack(
        80,
        50,
        number*2,
        0,
        unitAttackConstParams
    );

    auto attacksMap = new std::unordered_map<AttackClass, UnitAttack*>;
    auto classImmune = new std::unordered_map<unsigned int, ImmuneCategory>;
    auto sourceImmune = new std::unordered_map<unsigned int, ImmuneCategory>;
    auto hasClassImmune = new std::unordered_map<unsigned int, bool>;
    auto hasSourceImmune = new std::unordered_map<unsigned int, bool>;

    auto emptyUnit = new Unit(
        unitConstParams,
        false,
        false,
        false,
        false,
        false,
        10000,
        0,
        unitAttack1,
        nullptr,
        attacksMap,
        0,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        1,
        classImmune,
        sourceImmune,
        hasClassImmune,
        hasSourceImmune,
        false
    );

    return emptyUnit;
}