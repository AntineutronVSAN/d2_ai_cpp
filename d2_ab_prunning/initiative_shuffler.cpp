#include "initiative_shuffler.h"
#include <algorithm>
#include <random>
#include <unordered_map>

InitiativeShuffler::InitiativeShuffler(RandomExponentialDist* random)
{
	this->random = random; 
}

void InitiativeShuffler::shuffleAndSort(std::vector<Unit*>* unitsRef, RollConfig* rollConfig)
{

	auto rng = std::default_random_engine{}; // TODO Внедрение зависимостей

	std::shuffle(std::begin(*unitsRef), std::end(*unitsRef), rng);


	auto context = std::unordered_map<std::string, int>{};

	for (int i = 0; i < unitsRef->size(); i++) {
		auto currentUnit = (*unitsRef)[i];
		if ((i >= 0) & (i <= 5)) {
			// Топ команда
			if (rollConfig->topTeamMaxIni) {
				context[currentUnit->unitConstParams->unitWarId] = currentUnit->attack1->initiative + MAX_INITIATIVE_ROLL;
			}
			else {
				// Случайная прибавка 
				context[currentUnit->unitConstParams->unitWarId] = currentUnit->attack1->initiative + this->random->getNextInt(MAX_INITIATIVE_ROLL);
			}
		}
		else if ((i >= 6) & (i <= 11)) {
			// Бот команда
			if (rollConfig->bottomTeamMaxIni) {
				context[currentUnit->unitConstParams->unitWarId] = currentUnit->attack1->initiative + MAX_INITIATIVE_ROLL;
			}
			else {
				// Случайная прибавка 
				context[currentUnit->unitConstParams->unitWarId] = currentUnit->attack1->initiative + this->random->getNextInt(MAX_INITIATIVE_ROLL);
			}
		}
	}

	auto comp = [&](Unit* a, Unit* b) {
		return (context[a->unitConstParams->unitWarId] > context[b->unitConstParams->unitWarId]);
	};

	sort(unitsRef->begin(), unitsRef->end(), comp);

}
