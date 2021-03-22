#include "intermediateCode.h"

int MC::temp_order = 0;
int label_order = 0;

std::vector<mcode> MC::middleCode;

void MC::addIMC(std::string rst, std::string op, std::string op1, std::string op2) {
	mcode temp;
	temp.rst = rst;
	temp.op = op;
	temp.num1 = op1;
	temp.num2 = op2;
	MC::middleCode.push_back(temp);
}

std::string MC::genTemp(std::string curFunc, int lineNum, int type) {
	auto name = MC::temphd + std::to_string(++MC::temp_order);
	ST::addSymbol(curFunc, name, type, VAR, 0, 0,lineNum);
	return name;
}

std::string MC::genLabel() {
	return MC::labelhd + std::to_string(++label_order);
}