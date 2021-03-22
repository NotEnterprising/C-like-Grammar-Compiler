#pragma once
#include <string>
#include <map>
#include <vector>
#define INT 1
#define CHAR 2
#define VOID 3
#define CONST 1
#define VAR  2
#define ARRAY  3
#define FUNC  4
#define PARA  5

typedef struct {
	std::string name;
	int type;
	int kind;
	int length;
	int length1;
	int lineNum;	
	int addr = -1;
	int reg = -1;	
}tableItem;


extern std::map<std::string, tableItem> globalSymbolTable;
extern std::map<std::string, std::map<std::string, tableItem>> funcSymbolTable;
extern std::map<std::string, int>  outputStr;
extern std::map<std::string, std::map<std::string, std::vector<int>>> arrayAssignTable;

namespace ST {
	tableItem* lookUp(const std::string currentFuncIn, const std::string nameInput, bool local);
	bool isGlobalIdentifier(const std::string currentFunc, std::string str);
	std::map<std::string, tableItem> lookUpFunc(const std::string funcname);
	int lookUpPara(std::map<std::string, tableItem> funcSymbol, int count);

	int addStr(std::string& value);
	void addSymbol(std::string currentFunc, std::string nameInput, int typeIn, int kindIn, int lengthIn, int lengthIn1, int lineNumIn);
	void addArrayAssign(std::string currentFuncIn, std::string nameIn, std::vector<int> v);
}