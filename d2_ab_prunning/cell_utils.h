#pragma once

#include <unordered_map>


const std::unordered_map<int, int> NEAREST_MAP_CONST = std::unordered_map<int, int>(
	{ 
		{ 0, 3 },
		{ 1, 4 },
		{ 2, 5 },
		{ 3, 0 },
		{ 4, 1 },
		{ 5, 2 },
		{ 6, 9 },
		{ 7, 10 },
		{ 8, 11 },
		{ 9, 6 },
		{ 10, 7 },
		{ 11, 8 } 
	}
);
const std::unordered_map<int, int> BIG_UNIT_FIXED_POS_CONST = std::unordered_map<int, int>(
	{
		{ 0, 3 },
		{ 1, 4 },
		{ 2, 5 },
		{ 3, 3 },
		{ 4, 4 },
		{ 5, 5 },
		{ 6, 6 },
		{ 7, 7 },
		{ 8, 8 },
		{ 9, 6 },
		{ 10, 7 },
		{ 11, 8 }
	}
);

const bool TOP_TEAM_CHECK_CONST[12] = {
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	false,
	false,
	false,
	false,
};

const std::unordered_map<int, int> BEHIND_INDEXES_CONST = std::unordered_map<int, int>(
	{
		{ 3, 0 },
		{ 4, 1 },
		{ 5, 2 },
		{ 6, 9 },
		{ 7, 10 },
		{ 8, 11 },
	}
);

const int CELL_INDEX_LINE_NUMBER_CONST[12] = {
	0,0,0,1,1,1,2,2,2,3,3,3
};

class CellUtils
{
public:
	/// <summary>
	/// Относится ли ячейка index к верхней команде
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	bool checkIsTopTeam(int index);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool findNearestTarget(int index, int target, bool cellHasUnit[12], 
		bool directon, bool topFrontEmpty, bool botFrontEmpty,
		int currentRecursionLevel
		);

	int getLineNumber(int index);
};

