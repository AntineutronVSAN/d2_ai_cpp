#include "cell_utils.h"
#include <cassert>

bool CellUtils::checkIsTopTeam(int index)
{
	assert(index < 12);
	return TOP_TEAM_CHECK_CONST[index];
}

bool CellUtils::findNearestTarget(
	int index, 
	int target,
	bool cellHasUnit[12], 
	bool direction, 
	bool topFrontEmpty,
	bool botFrontEmpty, 
	int currentRecursionLevel)
{
	currentRecursionLevel++;
	assert(currentRecursionLevel < 100);
	assert((index >= 0) && (index <= 11));

	int currentIndex = index;
	int directionIncrement = direction ? 3 : -3;

	currentIndex += directionIncrement;
	int currentLineNumber = getLineNumber(currentIndex);

	if (direction ? checkIsTopTeam(currentIndex) : !checkIsTopTeam(currentIndex)) {
		// Попали на линию своих милишников
		if (direction ? !topFrontEmpty : !botFrontEmpty) {
			return false;
		}
		// Фронт пустой, идём дальше
		return findNearestTarget(
			currentIndex,
			target,
			cellHasUnit,
			direction,
			topFrontEmpty,
			botFrontEmpty,
			currentRecursionLevel
		);
	}
	if (currentIndex == target) {
		return true;
	}
	// Если фронт чужой стороны пустой, идём дальше
	// также нельзя выходить за пределы
	if ((direction ? botFrontEmpty : topFrontEmpty) &&
		(((currentIndex + directionIncrement) >= 0) &&
			((currentIndex + directionIncrement) <= 11))) {
		return findNearestTarget(
			currentIndex,
			target,
			cellHasUnit,
			direction,
			topFrontEmpty,
			botFrontEmpty,
			currentRecursionLevel
			);
	}

	// В стороны допускается только один шаг
	int leftIndex = currentIndex - 1;
	bool useLeftIndex = true;
	int rightIndex = currentIndex + 1;
	bool useRightIndex = true;

	int leftIndexLine = getLineNumber(leftIndex);
	int rightIndexLine = getLineNumber(rightIndex);

	// -1 Вернёт, если выпали за допустимые диапазоны
	// Если переход на следующую линию, то индекс не считается
	useLeftIndex = leftIndexLine != -1 && leftIndexLine == currentLineNumber;
	useRightIndex = rightIndexLine != -1 && rightIndexLine == currentLineNumber;

	// Хоть куда-то должно быть можно пойти дальше
	assert(useLeftIndex | useRightIndex);

	// Если живой юнит не найден, тогда считаем, что он далеко и до него можно добраться
	bool unitFound = false;

	if (useLeftIndex && !unitFound) {
		unitFound = cellHasUnit[leftIndex];
	}
	if (useRightIndex && !unitFound) {
		unitFound = cellHasUnit[rightIndex];
	}

	// Не забываем смотреть наличие юнита в текущей ячейке
	if (!unitFound) {
		unitFound = cellHasUnit[currentIndex];
	}
	if (!unitFound) {
		return currentLineNumber == getLineNumber(target);
	}
	return ((leftIndex == target) && useLeftIndex) |
		((rightIndex == target) && useRightIndex);

}

int CellUtils::getLineNumber(int index)
{
	if ((index < 0) || (index > 11)) return -1;
	return CELL_INDEX_LINE_NUMBER_CONST[index];
}
