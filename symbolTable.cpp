#include "symbolTable.h"
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>

std::map<std::string, tableItem> globalSymbolTable;
std::map<std::string, std::map<std::string, tableItem>> funcSymbolTable;
std::map<std::string, int>  outputStr;
std::map<std::string, std::map<std::string, std::vector<int>>> arrayAssignTable;

int strCount = 0;

int ST::addStr(std::string& value) {
	if (outputStr.count(value) == 0) {
		outputStr.insert(std::pair<std::string, int>(value, ++strCount));
		return strCount;
	}
	return outputStr[value];
}

void ST::addSymbol(std::string currentFuncIn, std::string nameInput, int typeIn, int kindIn, int lengthIn, int lengthIn1,int lineNumIn) {
	std::string nameIn=nameInput;
	std::string currentFunc = currentFuncIn;
	transform(nameIn.begin(), nameIn.end(), nameIn.begin(), ::tolower);
	transform(currentFunc.begin(), currentFunc.end(), currentFunc.begin(), ::tolower);
	tableItem tempItem;
	tempItem.name = nameIn;
	tempItem.type = typeIn;
	tempItem.kind = kindIn;
	tempItem.length = lengthIn;
	tempItem.length1 = lengthIn1;
	tempItem.lineNum = lineNumIn;
	if (currentFunc == "") {
		if (globalSymbolTable.count(nameIn) == 0) {
			globalSymbolTable.insert(std::pair<std::string, tableItem>(nameIn, tempItem));
			if(kindIn==FUNC) {
				std::map<std::string, tableItem> m;
				funcSymbolTable.insert(std::pair<std::string, std::map<std::string, tableItem>>(nameIn, m));
			}
		}
	}
	else {
		if (funcSymbolTable.count(currentFunc) > 0) {
			if (funcSymbolTable[currentFunc].count(nameIn) == 0&&(currentFunc!=nameIn)) {
				funcSymbolTable[currentFunc].insert(std::pair<std::string, tableItem>(nameIn, tempItem));
				if (kindIn == PARA) {
					globalSymbolTable[currentFunc].length++;
				}
			}
		}
	}
}

void ST::addArrayAssign(std::string currentFunc, std::string nameIn, std::vector<int> v) {
	std::string currentFuncIn;
	transform(nameIn.begin(), nameIn.end(), nameIn.begin(), ::tolower);
	if (currentFunc =="") {
		currentFuncIn = "global";
	}
	else {
		currentFuncIn = currentFunc;
	}
	if (arrayAssignTable.count(currentFuncIn) > 0) {
		if (arrayAssignTable[currentFuncIn].count(nameIn) > 0) {
			std::vector<int> t;
			t.insert(t.end(), arrayAssignTable[currentFuncIn][nameIn].begin(),arrayAssignTable[currentFuncIn][nameIn].end());
			t.insert(t.end(), v.begin(), v.end());
			arrayAssignTable[currentFuncIn][nameIn] = t;
		}
		else {
			arrayAssignTable[currentFuncIn].insert(std::pair<std::string, std::vector<int>>(nameIn, v));
		}
	}
	else {
		std::map<std::string, std::vector<int>> t;
		arrayAssignTable.insert(std::pair<std::string, std::map<std::string, std::vector<int>>>(currentFuncIn, t));
		arrayAssignTable[currentFuncIn].insert(std::pair<std::string, std::vector<int>>(nameIn, v));
	}
}

tableItem* ST::lookUp(const std::string currentFuncIn, const std::string nameInput, bool local) {
	std::string nameIn = nameInput;
	std::string currentFunc = currentFuncIn;
	transform(nameIn.begin(), nameIn.end(), nameIn.begin(), ::tolower);
	transform(currentFunc.begin(), currentFunc.end(), currentFunc.begin(), ::tolower);
	if (currentFunc == "") {
		if (globalSymbolTable.count(nameIn) > 0) {
			return &globalSymbolTable[nameIn];
		}
		else {
			return nullptr;
		}
	}
	else {
		if (funcSymbolTable.count(currentFunc) > 0) {
			if (funcSymbolTable[currentFunc].count(nameIn) > 0) {
				return &funcSymbolTable[currentFunc][nameIn];
			}
			if (local&&(currentFunc==nameIn)) {
				return &globalSymbolTable[nameIn];
			}
		}
		return nullptr;
	}
}

bool ST::isGlobalIdentifier(const std::string currentFunc, std::string str) {
	tableItem* temp = ST::lookUp(currentFunc, str, true);
	if (temp == nullptr) {
		return true;
	}
	else {
		return false;
	}
}

std::map<std::string, tableItem> ST::lookUpFunc(const std::string funcname) {
	return funcSymbolTable[funcname];
}

int ST::lookUpPara(std::map<std::string, tableItem> funcSymbol, int count) {
	for (auto iter = funcSymbol.begin(); iter != funcSymbol.end(); ++iter)
	{
		if (iter->second.length == count && iter->second.kind == PARA)
		{
			return (iter->second.type);
		}
	}
	return 0;
}
