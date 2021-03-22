#pragma once
#include <string>
#include <map>
#include "symbolTable.h"
#include "intermediateCode.h"

constexpr auto regTmax = 8;
constexpr auto regSmax = 9;
constexpr auto sizeOfReg = 4;

namespace GOC {
	void middleCodeTOMips(std::string mipsfile);
}

struct runinfo
{
	bool CLK_use[regTmax + regSmax];
	bool ts_use[regTmax + regSmax];
	int CLK_ptr;
	std::string ts_content[regTmax + regSmax];
	std::map<std::string, tableItem> symbolTable;
};