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
		// ������ �� ����� ����� ����������
		if (direction ? !topFrontEmpty : !botFrontEmpty) {
			return false;
		}
		// ����� ������, ��� ������
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
	// ���� ����� ����� ������� ������, ��� ������
	// ����� ������ �������� �� �������
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

	// � ������� ����������� ������ ���� ���
	int leftIndex = currentIndex - 1;
	bool useLeftIndex = true;
	int rightIndex = currentIndex + 1;
	bool useRightIndex = true;

	int leftIndexLine = getLineNumber(leftIndex);
	int rightIndexLine = getLineNumber(rightIndex);

	// -1 ������, ���� ������ �� ���������� ���������
	// ���� ������� �� ��������� �����, �� ������ �� ���������
	useLeftIndex = leftIndexLine != -1 && leftIndexLine == currentLineNumber;
	useRightIndex = rightIndexLine != -1 && rightIndexLine == currentLineNumber;

	// ���� ����-�� ������ ���� ����� ����� ������
	assert(useLeftIndex | useRightIndex);

	// ���� ����� ���� �� ������, ����� �������, ��� �� ������ � �� ���� ����� ���������
	bool unitFound = false;

	if (useLeftIndex && !unitFound) {
		unitFound = cellHasUnit[leftIndex];
	}
	if (useRightIndex && !unitFound) {
		unitFound = cellHasUnit[rightIndex];
	}

	// �� �������� �������� ������� ����� � ������� ������
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
