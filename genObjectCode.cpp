#include "genObjectCode.h"
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>

struct parainfo { int offset; std::string func_name; };
#define FrameSize (currentFunc=="main"?localVarSize[currentFunc]:(localVarSize[currentFunc]+sizeOfReg*(regTmax+regSmax+1)))
std::string currentFunc = "";
std::map<std::string, int> localVarSize;
std::vector<std::string> mips;
int dataHead = 0x10010000;
runinfo currentInfo;
const std::string RETV0 = "RET_v0";

void dataSegment();
std::string regAppoint(std::string& var);
int regToIndex(std::string regNum);
bool opIsConst(std::string operation, int& rst);
int nextRegIndex(int i);
void initCurrentInfo();
std::string markToUseReg(std::string& str);
std::string markToGlobalIden(std::string& str);
std::string markToFunc(std::string& str);
std::string markToLabel(std::string& str);
std::string indexToTempReg(int index);
std::string markToString(std::string& str);
void modifyRegInfo(std::string& var, int& i);
std::string regSeek(std::string& v, bool is_rsrt);
void futureUseCheck(std::string opr, std::string reg, std::vector<mcode>::iterator o);
void writeToRam(std::string iden, std::string reg);
void funcBegin(mcode& c);
void exit(mcode& c);
void scan(mcode& c);
void print(mcode& c, std::vector<mcode>::iterator o);
void calculate(mcode& c, std::vector<mcode>::iterator o);
void assignAddress();
void call(mcode& c);
void push(mcode& c, std::vector<mcode>::iterator o);
void readArray(mcode& c, std::vector<mcode>::iterator o);
void saveArray(mcode& c, std::vector<mcode>::iterator o);
void label(mcode& c);
void goTo(mcode& c);
void funcEndReturn(mcode& c);
void cmp(mcode& c, std::vector<mcode>::iterator o);

void setLocalVar() {
	for (auto i = currentInfo.symbolTable.begin(); i != currentInfo.symbolTable.end(); i++) {
		if (i->first[0] != '#') {
			std::stringstream tempStream;
			if (i->second.kind == VAR) {
				tempStream << "li" << ' ' << "$t9" << ' ' << i->second.length;
				mips.push_back(tempStream.str());
				tempStream.str("");
				if (i->second.type == INT) {
					tempStream << "sw" << ' ' << "$t9" << " -" << (i->second.addr) << " (" << "$sp" << ")";
				}
				else {
					tempStream << "sb" << ' ' << "$t9"  " -" << (i->second.addr) << " (" << "$sp" << ")";
				}
				mips.push_back(tempStream.str());
				tempStream.str("");
			}
			else if (i->second.kind == ARRAY) {
				int len;
				if (i->second.length1 == 0) {
					len = i->second.length;
				}
				else {
					len = i->second.length * i->second.length1;
				}
				if (arrayAssignTable[currentFunc].count(i->first) > 0) {
					if (i->second.type == INT) {
						int address = i->second.addr;
						for (int j = 0; j < len; j++) {
							std::stringstream tempStream;
							tempStream << "li" << ' ' << "$t9" << ' ' << arrayAssignTable[currentFunc][i->first][j];
							mips.push_back(tempStream.str());
							tempStream.str("");
							tempStream << "sw" << ' ' << "$t9" << " -" << address << " (" << "$sp" << ")";
							mips.push_back(tempStream.str());
							tempStream.str("");
							address += 4;
						}
					}
					else {
						int address = i->second.addr;
						for (int j = 0; j < len; j++) {
							std::stringstream tempStream;
							tempStream << "li" << ' ' << "$t9" << ' ' << arrayAssignTable[currentFunc][i->first][j];
							mips.push_back(tempStream.str());
							tempStream.str("");
							tempStream << "sb" << ' ' << "$t9" << " -" << address << " (" << "$sp" << ")";
							mips.push_back(tempStream.str());
							tempStream.str("");
							address += 1;
						}
					}
				}
				else {
					if (i->second.type == INT) {
						int address = i->second.addr;
						for (int j = 0; j < len; j++) {
							std::stringstream tempStream;
							tempStream << "sw" << ' ' << "$0" << " -" << address << " (" << "$sp" << ")";
							mips.push_back(tempStream.str());
							tempStream.str("");
							address += 4;
						}
					}
					else {
						int address = i->second.addr;
						for (int j = 0; j < len; j++) {
							std::stringstream tempStream;
							tempStream << "sb" << ' ' << "$0" << " -" << address << " (" << "$sp" << ")";
							mips.push_back(tempStream.str());
							tempStream.str("");
							address += 1;
						}
					}
				}
			}
		}
	}
}

void GOC::middleCodeTOMips(std::string mipsfile) {
	dataSegment();
	assignAddress();
	mips.push_back(".text");

	mips.push_back(".globl main");
	mips.push_back("j main");
	for (auto iter = MC::middleCode.begin(); iter != MC::middleCode.end(); iter++) {
		if (iter->op == OP::SCAN) {
			scan(*iter);
		}
		else if (iter->op == OP::PRINT) {
			print(*iter,iter);
		}
		else if (iter->op == OP::ADD || iter->op == OP::SUB || iter->op == OP::MUL || iter->op == OP::DIV) {
			calculate(*iter, iter);
		}
		else if (iter->op == OP::FUNC_BEGIN) {
			funcBegin(*iter);
		}
		else if (iter->op == OP::EXIT) {
			exit(*iter);
		}
		else if (iter->op == OP::GREQ ||iter->op == OP::LES || iter->op == OP::LESEQ||iter->op == OP::EQU || iter->op == OP::NEQ ||iter->op == OP::GRT ) {
			cmp(*iter, iter);
		}
		else if (iter->op == OP::LABEL) {
			label(*iter);
		}
		else if (iter->op == OP::SAVEARR) {
			saveArray(*iter, iter);
		}
		else if (iter->op == OP::GOTO) {
			goTo(*iter);
		}
		else if (iter->op == OP::READARR) {
			readArray(*iter, iter);
		}
		else if (iter->op == OP::PUSH) {
			push(*iter, iter);
		}
		else if (iter->op == OP::CALL) {
			call(*iter);
		}
		else if (iter->op == OP::FUNC_END || iter->op == OP::RET) {
			funcEndReturn(*iter);
		}
	}
	
	mips.push_back("exit:");
	std::ofstream mipsout(mipsfile, std::ios::trunc | std::ofstream::ate);
	bool putTab = false;
	for (auto iter = mips.begin(); iter != mips.end(); iter++)
	{
		if ((*iter) == ".text")
		{
			putTab = true;
			mipsout << *iter << std::endl;
			continue;
		}
		if (putTab && (*iter) != ".globl main" && (*iter).find(':') == -1)
			mipsout << '\t';
		mipsout << *iter << std::endl;
	}
}

void label(mcode& c) {
	mips.push_back(markToLabel(c.rst) + ":");
}

void goTo(mcode& c) {
	mips.push_back("j " + markToLabel(c.rst));
}

void funcEndReturn(mcode& c) {
	if (c.op == OP::FUNC_END) {
		currentFunc = "";
	}
	mips.push_back("jr $ra");
}

void cmp(mcode& c, std::vector<mcode>::iterator o)
{
	auto cop = c.op;
	auto r1 = c.num1, r2 = c.num2;
	int v1, v2;
	bool r1const = opIsConst(r1, v1);
	bool r2const = opIsConst(r2, v2);
	std::string r1reg, r2reg;
	std::stringstream ss;
	if (cop == OP::EQU || cop == OP::NEQ)
	{
		bool is_beq = (cop == OP::EQU);
		if (r1const & r2const)
		{
			bool eq = v1 == v2;
			if (is_beq & eq || !is_beq & !eq)
				ss << "j" << " " << markToLabel(c.rst);
		}
		else if (!r1const & r2const)
		{
			r1reg = regSeek(r1, true);
			ss << (is_beq ? "beq" : "bne") << " " << r1reg << " " <<
				v2 << " " << markToLabel(c.rst);
		}
		else if (r1const & !r2const)
		{
			r2reg = regSeek(r2, true);
			ss << (is_beq ? "beq" : "bne") << " " << r2reg << " " <<
				v1 << " " << markToLabel(c.rst);
		}
		else if (!r1const & !r2const)
		{
			r1reg = regSeek(r1, true);
			r2reg = regSeek(r2, true);
			ss << (is_beq ? "beq" : "bne") << " " << r1reg << " " <<
				r2reg << " " << markToLabel(c.rst);
		}
	}
	else
	{
		if (r1const & r2const)
		{
			if ((cop == OP::GRT && v1 > v2) ||
				(cop == OP::GREQ && v1 >= v2) ||
				(cop == OP::LES && v1 < v2) ||
				(cop == OP::LESEQ && v1 <= v2))
				ss << "j" << " " << markToLabel(c.rst);
		}
		else if (!r1const & r2const)
		{
			r1reg = regSeek(r1, true);
			if (v2 == 0)
			{
				auto o = cop == OP::GRT ? "bgtz" :
					cop == OP::GREQ ? "bgez" :
					cop == OP::LES ? "bltz": "blez";
				ss << o << " " << r1reg << " " << markToLabel(c.rst);
			}
			else
			{
				auto o = cop == OP::GRT ? "bgt" :
					cop == OP::GREQ ? "bge" :
					cop == OP::LES ? "blt" : "ble";
				ss << o << " " << r1reg << " " <<
					v2 << " " << markToLabel(c.rst);
			}
		}
		else if (r1const & !r2const)
		{
			r2reg = regSeek(r2, true);
			if (v1 == 0)
			{
				auto o = cop == OP::GRT ? "bltz" :
					cop == OP::GREQ ? "blez" :
					cop == OP::LES ? "bgtz" : "bgez";
				ss << o << " " << r2reg << " " << markToLabel(c.rst);
			}
			else
			{
				auto o = cop == OP::GRT ? "blt" :
					cop == OP::GREQ ? "ble" :
					cop == OP::LES ? "bgt" : "bge";
				ss << o << " " << r2reg << " " << v1 << " " << markToLabel(c.rst);
			}
		}
		else if (!r1const & !r2const)
		{
			r1reg = regSeek(r1, true);
			r2reg = regSeek(r2, true);
			auto o = cop == OP::GRT ? "bgt" :
				cop == OP::GREQ ? "bge" :
				cop == OP::LES ? "blt" : "ble";
			ss << o << " " << r1reg << " " <<
				r2reg << " " << markToLabel(c.rst);
		}
	}
	mips.push_back(ss.str());
	if (!r1const)
		futureUseCheck(r1, r1reg, o);
	if (!r2const)
		futureUseCheck(r2, r2reg, o);
}

void assignAddress()
{
	for (auto i = funcSymbolTable.begin(); i != funcSymbolTable.end(); i++)
	{
		int addr = 0;
		for (auto param_count = 0; param_count < globalSymbolTable[i->first].length; param_count++)
		{
			for (auto j = i->second.begin(); j != i->second.end(); j++)
				if (j->second.kind == PARA && j->second.length == param_count)
				{
					j->second.addr = addr;
					addr += sizeOfReg;
				}
		}
		for (auto j = i->second.begin(); j != i->second.end(); j++)
			if (j->second.type == CHAR && j->second.kind != PARA && j->second.kind != CONST)
			{
				j->second.addr = addr;
				int len;
				if (j->second.length1 != 0) {
					len = j->second.length * j->second.length1;
				}
				else {
					len = j->second.length;
				}
				addr += (j->second.kind == VAR ? 1 : len);
			}
		addr = addr + 2*sizeOfReg - (addr & 0x00000003);
		for (auto j = i->second.begin(); j != i->second.end(); j++)
			if (j->second.type == INT && j->second.kind != PARA && j->second.kind != CONST)
			{
				j->second.addr = addr;
				int len;
				if (j->second.length1 != 0) {
					len = j->second.length * j->second.length1;
				}
				else {
					len = j->second.length;
				}
				addr += (j->second.kind == VAR ?sizeOfReg : len * sizeOfReg);
			}
		localVarSize.insert(std::pair<std::string, int>(i->first, addr));
	}
}

void calculate(mcode& c, std::vector<mcode>::iterator o)
{
	auto r1 = c.num1, r2 = c.num2;
	int v1, v2;
	bool r1const = opIsConst(r1, v1);
	bool r2const = opIsConst(r2, v2);
	std::string r1reg, r2reg; 
	std::stringstream tempStream;
	std::string rdreg;
	if (r1 == RETV0)
	{
		rdreg = regSeek(c.rst, false);
		tempStream << "move" << ' ' << rdreg << ' ' << "$v0";
		mips.push_back(tempStream.str());
		if (rdreg != "$v0")
			writeToRam(c.rst, rdreg);
		return;
	}
	if (!r1const & r2const)
	{
		r1reg = regSeek(r1, true);
		if (v2 == 0)
		{
			rdreg = regSeek(c.rst, false);
			if (c.op == OP::ADD || c.op == OP::SUB)
				tempStream << "move" << ' ' << rdreg << ' ' << r1reg;
			else if (c.op == OP::MUL || c.op == OP::DIV)
				tempStream << "move" << ' ' << rdreg << ' ' << "$0";
			mips.push_back(tempStream.str());
		}
		else
		{
			rdreg = regSeek(c.rst, false);
			auto o =
				c.op == OP::ADD ? "addu" :
				c.op == OP::SUB ? "subu" :
				c.op == OP::MUL ? "mul" : "div";
			/*
			if (o == "addu")
				tempStream << "addiu" << ' ' << rdreg << ' ' << r1reg << ' ' << v2;
			else if (o == "subu")
				tempStream << "addiu" << ' ' << rdreg << ' ' << r1reg << ' ' << -v2;
			else
				tempStream << o << ' ' << rdreg << ' ' << r1reg << ' ' << v2;*/
			int targetlevel = 0;
			if ((v2 & (v2 - 1)) == 0) {
				int index = v2;
				while (index >>= 1) {
					targetlevel++;
				}
			}
			if (o == "mul" && ((v2 & (v2 - 1)) == 0) && v2 > 0)
				tempStream << "sll" << ' ' << rdreg << ' ' << r1reg << ' ' << targetlevel;
			else if(o == "addu")
				tempStream << "addiu" << ' ' << rdreg << ' ' << r1reg << ' ' << v2;
			else if (o == "subu")
				tempStream << "addiu" << ' ' << rdreg << ' ' << r1reg << ' ' << -v2;
			else
				tempStream << o << ' ' << rdreg << ' ' << r1reg << ' ' << v2;
			mips.push_back(tempStream.str());
		}
	}
	else if (r1const & !r2const)
	{
		r2reg = regSeek(r2, true);
		if (v1 == 0)
		{
			rdreg = regSeek(c.rst, false);
			if (c.op == OP::ADD)
				tempStream << "move" << ' ' << rdreg << ' ' << r2reg;
			else if (c.op == OP::SUB)
				tempStream << "subu" << ' ' << rdreg << ' ' << "$0" << ' ' << r2reg;
			else if (c.op == OP::MUL || c.op == OP::DIV)
				tempStream << "move" << ' ' << rdreg << ' ' << "$0";
			mips.push_back(tempStream.str());
		}
		else
		{
			rdreg = regSeek(c.rst, false);
			if (c.op == OP::ADD)
				tempStream << "addiu" << ' ' << rdreg << ' ' << r2reg << ' ' << v1;
			else if (c.op == OP::SUB)
			{
				tempStream << "addiu" << ' ' << "$t9" << ' ' << r2reg << ' ' << (-v1);
				mips.push_back(tempStream.str());
				tempStream.str("");
				tempStream << "subu" << ' ' << rdreg << ' ' << "$0" << ' ' << "$t9";
			}
			else if (c.op == OP::MUL)
			{
				tempStream << "mul" << ' ' << rdreg << ' ' << r2reg << ' ' << v1;
			}
			else if (c.op == OP::DIV)
			{
				tempStream << "li" << ' ' << "$t9" << ' ' << v1;
				mips.push_back(tempStream.str());
				tempStream.str("");
				tempStream << "div" << ' ' << rdreg << ' ' << "$t9" << ' ' << r2reg;
			}
			mips.push_back(tempStream.str());
		}
	}
	else if (!r1const & !r2const)
	{

		r1reg = regSeek(r1, true);
		r2reg = regSeek(r2, true);
		rdreg = regSeek(c.rst, false);
		auto o =
			c.op == OP::ADD ? "addu" :
			c.op == OP::SUB ? "subu" :
			c.op == OP::MUL ? "mul" : "div";
		tempStream << o << ' ' << rdreg << ' ' << r1reg << ' ' << r2reg;
		mips.push_back(tempStream.str());
	}else if (r1const & r2const)
	{
		int val =
			c.op == OP::ADD ? v1 + v2 :
			c.op == OP::SUB ? v1 - v2 :
			c.op == OP::MUL ? v1 * v2 :
			c.op == OP::DIV ? v1 / v2 : 0;
		rdreg = regSeek(c.rst, false);
		if (val == 0)
			tempStream << "addu" << ' ' << rdreg << ' ' << "$0" << ' ' << "$0";
		else
			tempStream << "li" << ' ' << rdreg << ' ' << val;
		mips.push_back(tempStream.str());
	}
	if (rdreg != "$v0")
		writeToRam(c.rst, rdreg);
	if (!r1const)
		futureUseCheck(c.num1, r1reg, o);
	if (!r2const)
		futureUseCheck(c.num2, r2reg, o);
}

void print(mcode& c, std::vector<mcode>::iterator o)
{
	std::stringstream tempStream;
	if (c.rst == "int" || c.rst == "char")
	{
		int val; bool is_const;
		tempStream << "li" << " " << "$v0" << ' ' << (c.rst == "int" ? 1 : 11); 
		mips.push_back(tempStream.str());
		tempStream.str("");
		is_const = opIsConst(c.num1, val);
		if (is_const)
			tempStream << "li" << ' ' << "$a0" << ' ' << val;
		else
		{
			auto reg = regSeek(c.num1, true);
			tempStream << "move" << ' ' << "$a0" << ' ' << reg;
			futureUseCheck(c.num1, reg, o);
		}
	}
	else if (c.rst == "\n") {
		tempStream << "li" << ' ' << "$v0" << ' ' << 11;
		mips.push_back(tempStream.str());
		tempStream.str("");
		tempStream << "li" << ' ' << "$a0" << ' ' << 10;
	}
	else
	{
		tempStream << "li" << " " << "$v0" << ' ' << 4; 
		mips.push_back(tempStream.str());
		tempStream.str("");
		tempStream << "la" << ' ' << "$a0" << ' ' << markToString(c.num1);
	}
	mips.push_back(tempStream.str());
	tempStream.str("");
	tempStream << "syscall";
	mips.push_back(tempStream.str());
	tempStream.str("");
}

void scan(mcode& c)
{
	std::stringstream tempStream;
	if (c.rst == "int")
		tempStream << "li" << " " << "$v0" << ' ' << 5;
	else
	{
		tempStream << "li" << " " << "$v0" << ' ' << 12;
	}
	mips.push_back(tempStream.str());
	tempStream.str("");
	tempStream << "syscall"; 
	mips.push_back(tempStream.str()); 
	tempStream.str("");
	auto reg = regSeek(c.num1, false);
	tempStream << "move" << ' ' << reg << ' ' << "$v0"; 
	mips.push_back(tempStream.str());
	writeToRam(c.num1, reg);
}

void exit(mcode& c)
{
	mips.push_back("j exit");		// alter: li + syscall
}


void funcBegin(mcode& c) {
	mips.push_back((c.rst == "main" ? "" : "func_") + c.rst + ":");
	currentFunc = c.rst;
	initCurrentInfo();
	setLocalVar();
}

void writeToRam(std::string iden, std::string reg)
{
	if (*(iden.begin()) != '#')
	{
		std::stringstream tempStream;
		if (currentInfo.symbolTable.count(iden) > 0)
		{	
			tableItem t = currentInfo.symbolTable[iden];
			tempStream << (t.type == CHAR ? "sb" : "sw") << " "
				<< reg << " -" << (t.addr) << " (" << "$sp" << ")";
		}
		else
		{	
			tableItem t = globalSymbolTable[iden];
			tempStream << (t.type == CHAR ? "sb" : "sw") << " "
				<< reg << " " << markToGlobalIden(iden);
		}
		mips.push_back(tempStream.str());
		currentInfo.ts_content[regToIndex(reg)] = "";
		currentInfo.ts_use[regToIndex(reg)] = false;
	}
}

void futureUseCheck(std::string opr, std::string reg, std::vector<mcode>::iterator o)
{
	if (*opr.begin() != '#')
	{
		int index = regToIndex(reg);
		currentInfo.ts_use[index] = false;
		currentInfo.ts_content[index] = "";
		return;
	}
	auto i = o + 1;
	for (; i != MC::middleCode.end(); i++)
	{
		if ((*i).op == OP::FUNC_END || (*i).rst == opr)
		{
			int idx = regToIndex(reg);
			currentInfo.ts_use[idx] = false;
			currentInfo.ts_content[idx] = "";
			return;
		}
		if ((*i).num1 == opr || (*i).num2 == opr
			|| ((*i).op == OP::SCAN && (*i).num1 == opr))
			return;
	}
	if (i == MC::middleCode.end())
	{
		int idx = regToIndex(reg);
		currentInfo.ts_use[idx] = false;
	}
}

std::string regAppoint(std::string& var)
{

	auto stopPos = currentInfo.CLK_ptr;
	do
	{
		auto i = currentInfo.CLK_ptr;
		if (currentInfo.ts_content[i] == "")
		{
			modifyRegInfo(var, i);
			return indexToTempReg(i);
		}
		currentInfo.CLK_ptr = nextRegIndex(currentInfo.CLK_ptr);
	} while (currentInfo.CLK_ptr != stopPos);

	std::set<int> unUseReg;
	stopPos = currentInfo.CLK_ptr;
	do
	{
		auto i = currentInfo.CLK_ptr;
		if (currentInfo.ts_use[i] == false)
		{
			auto s = currentInfo.ts_content[i];
			if (*(s.begin()) == '#')
			{
				modifyRegInfo(var, i);
				return indexToTempReg(i);
			}
			unUseReg.insert(i);
		}
		currentInfo.CLK_ptr = nextRegIndex(currentInfo.CLK_ptr);
	} while (currentInfo.CLK_ptr != stopPos);
	if (!unUseReg.empty())
	{
		auto i = *unUseReg.begin();
		modifyRegInfo(var, i);
		return indexToTempReg(i);
	}
	while (currentInfo.CLK_use[currentInfo.CLK_ptr] != false)
	{
		if (currentInfo.CLK_use[currentInfo.CLK_ptr] == true)
		{
			currentInfo.CLK_use[currentInfo.CLK_ptr] = false;
		}
		currentInfo.CLK_ptr = nextRegIndex(currentInfo.CLK_ptr);
	}
	auto i = currentInfo.CLK_ptr;
	auto iden = currentInfo.ts_content[i];
	auto reg = indexToTempReg(i); 
	std::stringstream tempStream;
	if (currentInfo.symbolTable.count(iden) > 0)
	{	
		tableItem t = currentInfo.symbolTable[iden];
		auto addr = t.addr;
		tempStream << (t.type == CHAR ? "sb" : "sw") << " "
			<< reg << " -" << addr << " (" << "$sp" << ")";
	}
	else
	{	
		tableItem t = globalSymbolTable[iden];
		tempStream << (t.type == CHAR ? "sb" : "sw") << " "
			<< reg << " " << markToGlobalIden(iden);
	}
	mips.push_back(tempStream.str());
	modifyRegInfo(var, i);
	return reg;
}

std::string regSeek(std::string& v, bool is_rsrt)
{	
	if (v == RETV0)
		return "$v0";
	auto lookUpTs = markToUseReg(v);
	if (lookUpTs != "0")
	{
		currentInfo.ts_use[regToIndex(lookUpTs)] = true;
		return lookUpTs;
	}
	auto rd = regAppoint(v);
	if (!is_rsrt)
		return rd;
	std::stringstream tempStream;
	if (currentInfo.symbolTable.count(v) > 0)
	{	
		tableItem t = currentInfo.symbolTable[v];
		tempStream << (t.type == CHAR ? "lbu" : "lw") << " "
			<< rd << " -" << (t.addr) << " (" << "$sp" << ")";
	}
	else
	{	
		tableItem t =globalSymbolTable[v];
		tempStream << (t.type == CHAR ? "lbu" : "lw") << " "
			<< rd << " " << markToGlobalIden(v);
	}
	mips.push_back(tempStream.str());
	return rd;
}

void modifyRegInfo(std::string &var,int &i) {
	currentInfo.CLK_use[i] = true;
	currentInfo.ts_use[i] = true;
	currentInfo.ts_content[i] = var;
	currentInfo.CLK_ptr = nextRegIndex(i);
}

int regToIndex(std::string regNum) {
	if (regNum[1] == 't')
		return regNum[2] - '0';
	else if (regNum[1] == 's')
		return regNum[2] - '0' + regTmax;
	else if (regNum == "$fp")
		return regTmax + regSmax - 1;
	return -1;
}

bool opIsConst(std::string operation, int& rst) {
	auto temp = operation.begin();
	if (*(operation.begin()) == '\'')
	{
		rst = *(temp + 1);
		return true;
	}
	int num = 0;
	try
	{
		num = std::stoi(operation);
	}
	catch (const std::exception&)
	{
		return false;
	}
	rst = num;
	return true;
}

void saveArray(mcode& c, std::vector<mcode>::iterator o)
{	
	auto arr = c.rst,
		index = c.num1,
		value = c.num2; 
	std::stringstream ss;
	tableItem arr_iden; 
	bool local = false; 
	int idx, val;
	bool const_index = opIsConst(index, idx);
	bool const_value = opIsConst(value, val);
	if (currentInfo.symbolTable.count(arr) > 0)
	{
		arr_iden = currentInfo.symbolTable[arr];
		local = true;
	}
	else
	{
		arr_iden = globalSymbolTable[arr];
	}
	int elmt_size = 1; std::string SAVEcode = "sb";
	if (arr_iden.type == INT)
	{
		elmt_size = sizeOfReg;
		SAVEcode = "sw";
	}

	std::string valreg, idxreg;
	if (const_value)
	{
		ss << "li" << ' ' << "$t9" << ' ' << val; 
		mips.push_back(ss.str());
		ss.str("");
		valreg = "$t9";
	}
	else {
		valreg = regSeek(value, true);
	}
	if (local)
	{
		if (const_index)
		{
			ss << SAVEcode << ' ' << valreg << ' '<< -1 * (idx * elmt_size + arr_iden.addr) << " (" << "$sp" << ")"; 
			mips.push_back(ss.str());
		}
		else
		{
			idxreg = regSeek(index, true);
			if (elmt_size > 1)
			{
				ss << "sll" << ' ' << "$t8" << ' ' << idxreg << ' ' << "2"; 
				mips.push_back(ss.str()); 
				ss.str("");
				ss << "addiu" << ' ' << "$t8" << ' ' << "$t8" << ' ' << arr_iden.addr; 
				mips.push_back(ss.str());
				ss.str("");
			}
			else {
				ss << "addiu" << ' ' << "$t8" << ' ' << idxreg << ' ' << arr_iden.addr; 
				mips.push_back(ss.str());
				ss.str("");
			}
			ss << "subu" << ' ' << "$t8" << ' ' << "$sp" << ' ' << "$t8"; 
			mips.push_back(ss.str());
			ss.str("");
			ss << SAVEcode << ' ' << valreg << ' ' << '(' << "$t8" << ')'; 
			mips.push_back(ss.str());
			ss.str("");
		}
	}
	else
	{
		if (const_index)
		{
			ss << SAVEcode << ' ' << valreg << ' ' << markToGlobalIden(arr) << '+' << idx * elmt_size; 
			mips.push_back(ss.str());
		}
		else
		{
			idxreg = regSeek(index, true);
			if (elmt_size > 1)
			{
				ss << "sll" << ' ' << "$t8" << ' ' << idxreg << ' ' << "2"; 
				mips.push_back(ss.str()); 
				ss.str("");
			}
			ss << SAVEcode << ' ' << valreg << ' ' << markToGlobalIden(arr)<< " (" << (elmt_size > 1 ? "$t8" : idxreg) << ")"; 
			mips.push_back(ss.str()); 
			ss.str("");
		}
	}
	if (!const_index)
		futureUseCheck(index, idxreg, o);
	if (!const_value)
		futureUseCheck(value, valreg, o);
}

int nextRegIndex(int i) {
	return i + 1 < regTmax + regSmax ?(i + 1):(i + 1 - regTmax - regSmax);
}
void initCurrentInfo() {
	runinfo temp;
	for (auto iter = 0; iter < regTmax + regSmax; iter++)
	{
		temp.CLK_use[iter] = false;
		temp.ts_use[iter] = false;
		temp.ts_content[iter] = "";
	}
	temp.CLK_ptr = 0;
	temp.symbolTable = funcSymbolTable[currentFunc];
	currentInfo = temp;
}

std::string markToUseReg(std::string& str) {
	for (auto iter = 0; iter < regTmax + regSmax; iter++)
		if (currentInfo.ts_content[iter] == str)
			return indexToTempReg(iter);
	return "0";
}

std::string markToString(std::string& str) {
	return "string_" + str.erase(0, 1);
}

std::string markToGlobalIden(std::string& str) {
	return "_" + str;
}
std::string markToFunc(std::string& str) {
	return "func_" + str;
}

std::string markToLabel(std::string& str) {
	return "label" + str.erase(0, 1);
}

std::string indexToTempReg(int index) {
	auto ret = index < regTmax ? "$t" + std::to_string(index) :
		"$s" + std::to_string(index - regTmax);
	if (ret == "$s8")
		ret = "$fp";
	return ret;
}

void readArray(mcode& c, std::vector<mcode>::iterator o)
{	
	auto arr = c.num1,
		index = c.num2,
		dst = c.rst;
	tableItem arr_iden; bool local = false; int idx;
	bool const_index = opIsConst(index, idx);
	std::string reg_index;
	if (currentInfo.symbolTable.count(arr) > 0)
	{
		arr_iden = currentInfo.symbolTable[arr];
		local = true;
	}
	else
	{
		arr_iden = globalSymbolTable[arr];
	}
	int elmt_size = 1;
	if (arr_iden.type == INT)
		elmt_size = sizeOfReg;
	std::string rdreg;
	if (local)
	{
		std::stringstream ss;
		if (const_index)
		{
			rdreg = regSeek(dst, false);
			ss << (arr_iden.type == INT ? "lw" : "lbu")
				<< " " << rdreg << " -" << arr_iden.addr + idx * elmt_size << "(" << "$sp" << ")";
			mips.push_back(ss.str());
		}
		else
		{
			reg_index = regSeek(index, true);
			rdreg = regSeek(dst, false);
			if (arr_iden.type == INT)
			{
				ss << "sll" << " " << "$t9" << " " << reg_index << ' ' << 2; 
				mips.push_back(ss.str());
				ss.str("");
				ss << "addiu" << " " << "$t9" << " " << "$t9" << " " << arr_iden.addr; 
				mips.push_back(ss.str());
				ss.str("");
				ss << "subu" << ' ' << "$t9" << ' ' << "$sp" << ' ' << "$t9";
				mips.push_back(ss.str());
				ss.str("");
				ss << "lw" << ' ' << rdreg << ' ' << '(' << "$t9" << ')'; 
				mips.push_back(ss.str());
				ss.str("");
			}
			else
			{
				ss << "addiu" << " " << "$t9" << " " << reg_index << " " << arr_iden.addr; 
				mips.push_back(ss.str());
				ss.str("");
				ss << "subu" << ' ' << "$t9" << ' ' << "$sp" << ' ' << "$t9";
				mips.push_back(ss.str());
				ss.str("");
				ss << "lbu" << ' ' << rdreg << ' ' << '(' << "$t9" << ')'; 
				mips.push_back(ss.str());
				ss.str("");
			}
		}
	}
	else
	{
		std::stringstream ss;
		if (const_index)
		{
			rdreg = regSeek(dst, false);
			ss << (arr_iden.type == INT ? "lw" : "lbu")
				<< " " << rdreg << " " << markToGlobalIden(arr) <<' '<< '+' << idx * elmt_size;
			mips.push_back(ss.str());
		}
		else
		{
			reg_index = regSeek(index, true);
			rdreg = regSeek(dst, false);
			if (arr_iden.type == INT)
			{
				ss <<"sll" << " " << "$t9" << " " << reg_index << ' ' << 2; 
				mips.push_back(ss.str());
				ss.str("");
				ss << "lw" << ' ' << rdreg << ' ' << markToGlobalIden(arr) << '(' << "$t9" << ')';
				mips.push_back(ss.str());
				ss.str("");
			}
			else {
				ss << "lbu" << ' ' << rdreg << ' ' << markToGlobalIden(arr) << '(' << reg_index << ')';
				mips.push_back(ss.str());
			}
		}
	}
	if (rdreg != "$v0")
		writeToRam(c.rst, rdreg);
	if (!const_index)
		futureUseCheck(index, reg_index, o);
}

void dataSegment() {
	mips.push_back(".data");
	for (auto iter = globalSymbolTable.begin(); iter != globalSymbolTable.end(); iter++) {
		if (iter->second.kind == VAR && iter->second.type == CHAR) {
			mips.push_back("_" + iter->first + ": .byte "+std::to_string(iter->second.length));
			iter->second.addr = dataHead;
			dataHead += sizeof(char);
		}
		else if (iter->second.kind == ARRAY && iter->second.type == CHAR) {
			int temp;
			if (iter->second.length1 == 0) {
				temp = iter->second.length;
			}
			else {
				temp = (iter->second.length) * (iter->second.length1);
			}
			if (arrayAssignTable["global"].count(iter->second.name)>0) {
				mips.push_back("_" + iter->first + ": ");
				for (int i = 0; i < temp; i++) {
					mips.push_back(".byte " + std::to_string(arrayAssignTable["global"][iter->first][i]));
				}
			}
			else {
				mips.push_back("_" + iter->first + " : .space " + std::to_string(temp));
			}
			iter->second.addr = dataHead;
			dataHead += temp;
		}
	}
	dataHead += sizeOfReg - dataHead & 0x00000003;
	mips.push_back(".align 2");
	for (auto iter = globalSymbolTable.begin(); iter != globalSymbolTable.end(); iter++)
	{
		if (iter->second.kind == VAR && iter->second.type == INT)
		{
			mips.push_back("_" + iter->first + " : .word " + std::to_string(iter->second.length));
			iter->second.addr = dataHead;
			dataHead += sizeof(int);
		}
		else if (iter->second.kind == ARRAY && iter->second.type == INT)
		{
			int temp;
			if (iter->second.length1 == 0) {
				temp = iter->second.length;
			}
			else {
				temp = (iter->second.length) * (iter->second.length1);
			}
			if (arrayAssignTable["global"].count(iter->second.name) > 0) {
				mips.push_back("_" + iter->first + ": ");
				for (int i = 0; i < temp; i++) {
					mips.push_back(".word " + std::to_string(arrayAssignTable["global"][iter->first][i]));
				}
			}
			else {
				mips.push_back("_" + iter->first + " : .space " + std::to_string(temp * sizeof(int)));
			}
			iter->second.addr = dataHead;
			dataHead += temp * sizeof(int);
		}
	}
	for (auto iter = outputStr.begin(); iter != outputStr.end(); iter++)
	{
		mips.push_back("string_" + std::to_string(iter->second) + " : .asciiz " + "\"" + iter->first + "\"");
		dataHead += iter->first.length();
	}
}

std::stack<parainfo> param_record;
void push(mcode& c, std::vector<mcode>::iterator o)
{
	int offset;
	if (param_record.empty())
		offset = FrameSize;
	else
		offset = param_record.top().offset + sizeOfReg;
	int v;
	bool para_const = opIsConst(c.num1, v);
	std::string save_reg; 
	std::stringstream ss;
	if (para_const)
	{
		ss << "li" << " " << "$t9" << " " << v;
		save_reg = "$t9";
		mips.push_back(ss.str());
		ss.str("");
	}
	else
	{
		save_reg = regSeek(c.num1, true);
		futureUseCheck(c.num1, save_reg, o);
	}
	ss << "sw" << " " << save_reg << " -" << offset << " (" << "$sp" << ")";
	mips.push_back(ss.str());
	struct parainfo t;	
	t.func_name = c.rst;  
	t.offset = offset;
	param_record.push(t);
}

void call(mcode& c)
{
	std::string func_name = c.rst;
	int last_push_offset = 0;
	if (param_record.empty()) {
		last_push_offset = FrameSize - sizeOfReg;
	}
	else {
		last_push_offset = param_record.top().offset;
	}
	int stack_ptr = last_push_offset + sizeOfReg -globalSymbolTable[func_name].length * sizeOfReg +localVarSize[func_name];

	for (auto i = 0; i < regTmax + regSmax; i++)
		if (currentInfo.ts_use[i])
		{
			std::stringstream ss;
			ss << "sw" << " " << indexToTempReg(i)<< " -" << stack_ptr << " (" << "$sp" << ")";
			mips.push_back(ss.str());;
			stack_ptr += sizeOfReg;
		}
	stack_ptr = last_push_offset + sizeOfReg - globalSymbolTable[func_name].length * sizeOfReg +localVarSize[func_name] +sizeOfReg * (regTmax + regSmax);

	std::stringstream ss; 
	ss << "sw" << " " << "$ra" << " -" << stack_ptr << " (" << "$sp" << ")"; 
	mips.push_back(ss.str()); 
	stack_ptr += sizeOfReg;
	for (auto i = 0; i < globalSymbolTable[func_name].length; i++)
	{
		param_record.pop();
	}
	int sp_change = 0;
	if (param_record.empty())
		sp_change = FrameSize;
	else
		sp_change = param_record.top().offset + sizeOfReg;
	ss.str("");
	ss << "addiu"<< " " << "$sp" << " " << "$sp" << " " << (-1 * sp_change); 
	mips.push_back(ss.str());
	ss.str(""); 
	ss << "jal" << " " << markToFunc(c.rst); 
	mips.push_back(ss.str());


	ss.str("");
	ss << "addiu" << " " << "$sp" << " " << "$sp" << " " << sp_change; 
	mips.push_back(ss.str()); 

	stack_ptr -= sizeOfReg;
	ss.str(""); 
	ss << "lw" << " " << "$ra" << " -" << stack_ptr << " (" << "$sp" << ")"; 
	mips.push_back(ss.str());
	stack_ptr = last_push_offset + sizeOfReg -globalSymbolTable[func_name].length * sizeOfReg +localVarSize[func_name];
	for (auto i = 0; i < regTmax + regSmax; i++)
	{
		if (currentInfo.ts_use[i])
		{
			std::stringstream ss;
			ss << "lw" << " " << indexToTempReg(i)
				<< " -" << stack_ptr << " (" << "$sp" << ")";
			mips.push_back(ss.str());;
			stack_ptr += sizeOfReg;
		}
	}

}