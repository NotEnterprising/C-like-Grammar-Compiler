#pragma once
#include<string>
#include<vector>
#include "symbolTable.h"

typedef struct
{
	std::string rst;
	std::string op;
	std::string num1;
	std::string num2;
} mcode;

namespace OP {
	constexpr auto EXIT = "exit";
	constexpr auto PRINT = "print";
	constexpr auto SCAN = "scanf";
	constexpr auto MUL = "*";
	constexpr auto DIV = "/";
	constexpr auto ADD = "+";
	constexpr auto SUB = "-";
	constexpr auto FUNC_END = "func_end";
	constexpr auto FUNC_BEGIN = "func_begin";
	constexpr auto EQU="==";
	constexpr auto NEQ="!=";
	constexpr auto GRT=">";
	constexpr auto GREQ=">=";
	constexpr auto LES="<";
	constexpr auto LESEQ="<=";
	constexpr auto SAVEARR="[]=";
	constexpr auto READARR="=[]";
	constexpr auto PUSH="push";
	constexpr auto CALL = "call";
	constexpr auto RET = "ret";
	constexpr auto GOTO = "goto";
	constexpr auto LABEL = "label";
}

namespace MC {
	constexpr auto labelhd = "%";
	constexpr auto temphd = "#";
	constexpr auto strhd = "~";
	extern std::vector<mcode> middleCode;
	extern int temp_order;

	std::string genTemp(std::string curFunc, int lineNum, int type);
	std::string genLabel();

	bool is_operand_num(std::string& s, int& value);
	void addIMC(std::string rst, std::string op, std::string op1, std::string op2);
}
