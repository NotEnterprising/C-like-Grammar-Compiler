#include <iostream>
#include <string>
#include <iomanip>
#include "grammarAnalyzer.h"
#include <unordered_map>
#include "symbolTable.h"
#include <algorithm>

static std::unordered_map<std::string, bool> funcIsReturnMap;

grammarAnalyzer::grammarAnalyzer(lexicalanalysis& lexicalanalyzer, std::ofstream& output, std::vector<std::string>& vectorin,error & Error)
	:mylexicalanalyzer(lexicalanalyzer), fileOutput(output), printvector(vectorin) ,myerror(Error){
	currentFunc = "";
	hasReturn = false;
}

void grammarAnalyzer::startAnalyze() {
	mylexicalanalyzer.initial();
	getSym();
	program();
}

void grammarAnalyzer::program() {
	bool varFlag = false;
	bool flag;
	if (keyword == CONSTTK) {
		constStatement();
	}

	if (keyword == INTTK || keyword == CHARTK || keyword == VOIDTK) {
		while (keyword == INTTK || keyword == CHARTK || keyword == VOIDTK || keyword == LPARENT) {
			if (keyword == INTTK || keyword == CHARTK) {
				if (keyword == INTTK) {
					flag = true;
				}
				else if (keyword == CHARTK) {
					flag = false;
				}
				getSym();
				if (keyword == IDENFR) {
					funcName = *mylexicalanalyzer.stringPointer;
					getSym();
					if (keyword == LBRACK || keyword == COMMA || keyword == SEMICN || keyword == ASSIGN) {
						if (!varFlag) {
							varFlag = true;
							flag=varStatement(flag,funcName);
						}
						else {

						}
					}
					else if (keyword == LPARENT) {
						printGrammarAnswer("<声明头部>")
							funcIsReturnMap.insert(std::pair<std::string, bool>(funcName, true));
						if (flag) {
							returnFuncDefinition(funcName,INT);
						}
						else {
							returnFuncDefinition(funcName, CHAR);
						}
					}
					else {

					}
				}
				else {

				}
			}
			else if (keyword == VOIDTK) {
				getSym();
				if (keyword == IDENFR) {
					funcName = *mylexicalanalyzer.stringPointer;
					funcIsReturnMap.insert(std::pair<std::string, bool>(funcName, false));
					getSym();
					noReturnFuncDefinition(funcName);
				}
				else if (keyword == MAINTK) {
					break;
				}
			}
			else {
				printvector.insert(printvector.begin() + printvector.size() - 2, "<变量说明>");
				printGrammarAnswer("<声明头部>")
					funcIsReturnMap.insert(std::pair<std::string, bool>(funcName, true));
				if (flag) {
					returnFuncDefinition(funcName, INT);
				}
				else {
					returnFuncDefinition(funcName, CHAR);
				}
			}

		}
	}
	else {}
	if (keyword == MAINTK) {
		funcName = "main";
		currentFunc = "";
		hasReturn = false;
		if (ST::lookUp(currentFunc, funcName, true) == nullptr) {
			ST::addSymbol(currentFunc, funcName, VOID, FUNC, 0,0, mylexicalanalyzer.preLine);
		}
		else {
			errorPrint('b', mylexicalanalyzer.currentLine);
		}
		MC::addIMC("main", OP::FUNC_BEGIN, "0", "0");
		currentFunc = funcName;
		getSym();
	}
	else {}

	if (keyword == LPARENT) {
		getSym();
		if (keyword != RPARENT) {
			errorPrint('l', mylexicalanalyzer.currentLine);
		}else{ getSym(); }
		if (keyword == LBRACE) {
			getSym();
			compoundSentence();
			if (keyword == RBRACE) {
				getSym();
			}
		}
	}
	MC::addIMC("0", OP::EXIT, "0", "0");
	printGrammarAnswer("<主函数>")
		printGrammarAnswer("<程序>")
}

void grammarAnalyzer::errorPrint(char a,int line) {
	myerror.errorPrint(a, line);
}

void grammarAnalyzer::constStatement() {
	do {
		if (keyword == CONSTTK) {
			getSym();
		}
		else {}
		constDefinition();
		if (keyword == SEMICN) {
			getSym();
		}
		else { errorPrint('k', mylexicalanalyzer.currentLine);}
	} while (keyword == CONSTTK);
	printGrammarAnswer("<常量说明>")
}

void grammarAnalyzer::constDefinition() {
	int intTemp;
	std::string name;
	if (keyword == INTTK) {
		getSym();
		if (keyword == IDENFR) {
			name = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		else {}
		if (keyword == ASSIGN) {
			getSym();
		}
		else {}
		std::string value;
		intTemp=integer(value);
		if (ST::lookUp(currentFunc, name, true) == nullptr) {
			ST::addSymbol(currentFunc, name, INT, CONST, intTemp,0, mylexicalanalyzer.preLine);
		}
		else {
			errorPrint('b', mylexicalanalyzer.preLine);
		}
		while (keyword == COMMA) {
			getSym();
			if (keyword == IDENFR) {
				name = *mylexicalanalyzer.stringPointer;
				getSym();
			}
			else {}
			if (keyword == ASSIGN) {
				getSym();
			}
			else {}
			intTemp = integer(value);
			if (ST::lookUp(currentFunc, name, true) == nullptr) {
				ST::addSymbol(currentFunc, name, INT, CONST, intTemp,0, mylexicalanalyzer.preLine);
			}else {
				errorPrint('b', mylexicalanalyzer.preLine);
			}
		}
	}
	else if (keyword == CHARTK) {
		getSym();
		if (keyword == IDENFR) {
			name = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		else {}
		if (keyword == ASSIGN) {
			getSym();
		}
		else {}
		if (keyword == CHARCON) {
			char temp = (*mylexicalanalyzer.stringPointer)[0];
			if (ST::lookUp(currentFunc, name, true) == nullptr) {
				ST::addSymbol(currentFunc, name, CHAR, CONST, temp, 0,mylexicalanalyzer.preLine);
			}
			else {
				errorPrint('b', mylexicalanalyzer.currentLine);
			}
			getSym();
		}
		else {}
		while (keyword == COMMA) {
			getSym();
			if (keyword == IDENFR) {
				name = *mylexicalanalyzer.stringPointer;
				getSym();
			}
			else {}
			if (keyword == ASSIGN) {
				getSym();
			}
			else {}
			if (keyword == CHARCON) {
				char temp = (*mylexicalanalyzer.stringPointer)[0];
				if (ST::lookUp(currentFunc, name, true) == nullptr) {
					ST::addSymbol(currentFunc, name, CHAR, CONST, temp, 0,mylexicalanalyzer.preLine);
				}
				else {
					errorPrint('b', mylexicalanalyzer.currentLine);
				}
				getSym();
			}
			else {}
		}
	}
	else {}
	printGrammarAnswer("<常量定义>")
}

int grammarAnalyzer::unsignedInteger(std::string &value) {
	int unsignednIntTemp=0;
	if (keyword == INTCON) {
		unsignednIntTemp = mylexicalanalyzer.intValue;
		value = *mylexicalanalyzer.stringPointer;
		getSym();
	}
	else {
		errorPrint('o', mylexicalanalyzer.currentLine);
		getSym();
	}
	printGrammarAnswer("<无符号整数>")
	return unsignednIntTemp;
}

int grammarAnalyzer::integer(std::string &value) {
	int intTemp;
	std::string temp;
	if (keyword == PLUS) {
		getSym();
		intTemp=unsignedInteger(temp);
		value = temp;
	}
	else if (keyword == MINU) {
		getSym();
		intTemp=-unsignedInteger(temp);
		value = "-" + temp;
	}
	else {
		intTemp=unsignedInteger(temp);
		value = temp;
	}
	printGrammarAnswer("<整数>")
	return intTemp;
}

int grammarAnalyzer::varStatement(bool flag,std::string identifier) {
	varDefinition(flag,identifier);
	if (keyword == SEMICN) {
		getSym();
	}
	else { errorPrint('k', mylexicalanalyzer.currentLine); }
	while (keyword == INTTK || keyword == CHARTK) {
		bool temp;
		temp = (keyword == INTTK) ? true : false;
		getSym();
		if (keyword == IDENFR) {
			funcName = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		else {}
		varDefinition(temp,funcName);
		if (keyword == LPARENT) {
			return temp;
		}
		if (keyword == SEMICN) {
			getSym();
		}
		else { errorPrint('k', mylexicalanalyzer.currentLine); }
	}
	printGrammarAnswer("<变量说明>")
		return 0;
}

void grammarAnalyzer::twoDimensionalArray() {
	if (keyword == LBRACK) {
		getSym();
		std::string value;
		unsignedInteger(value);
		arraySizej = mylexicalanalyzer.intValue;
		if (keyword == RBRACK) {
			getSym();
		}
		else {
			errorPrint('m', mylexicalanalyzer.currentLine);
		}
	}
}

void grammarAnalyzer::oneDimensionalArray() {
	if (keyword == LBRACK) {
		getSym();
		std::string value;
		unsignedInteger(value);
		arraySizei = mylexicalanalyzer.intValue;
		if (keyword == RBRACK) {
			getSym();
		}
		else {
			errorPrint('m', mylexicalanalyzer.currentLine);
		}
	}
}

void grammarAnalyzer::oneDimensionalAssignment(std::string identifier,bool flag, int arraySize) {
	std::vector<int> v;
	
	if (keyword == LBRACE) {
		int count = 0;
		if (flag) {
			getSym();
			if (keyword != RBRACE) {
				std::string value;
				int t=integer(value);
				v.emplace_back(t);
				count++;
				printGrammarAnswer("<常量>")
					while (keyword == COMMA) {
						if (keyword == COMMA) {
							getSym();
						}
						int t=integer(value);
						v.emplace_back(t);
						count++;
						printGrammarAnswer("<常量>")
					}
			}
		}
		else {
			getSym();
			if (keyword != RBRACE) {
				if (keyword == CHARCON) {
					v.emplace_back(mylexicalanalyzer.intValue);
					getSym();
					printGrammarAnswer("<常量>")
						count++;
				}
				else {
					errorPrint('o', mylexicalanalyzer.currentLine);
					getSym();
					count++;
				}
				while (keyword == COMMA) {
					if (keyword == COMMA) {
						getSym();
					}
					if (keyword == CHARCON) {
						v.emplace_back(mylexicalanalyzer.intValue);
						getSym();
						printGrammarAnswer("<常量>")
							count++;
					}
					else {
						errorPrint('o', mylexicalanalyzer.preLine);
						getSym();
						count++;
					}
				}
			}
		}
		if (count != arraySize) {
			errorPrint('n', mylexicalanalyzer.preLine);
		}
	}
	else {}
	if (keyword == RBRACE) {
		getSym();
	}
	else {}
	ST::addArrayAssign(currentFunc, identifier, v);
}
void grammarAnalyzer::twoDimensionalAssignment(std::string identifier,bool flag, int arraySizeOfI, int arraySizeOfJ) {
	int count = 0;
	if (keyword == LBRACE) {
		getSym();
	}
	if (keyword != RBRACE) {
		oneDimensionalAssignment(identifier,flag, arraySizeOfJ);
		count++;
		while (keyword == COMMA) {
			if (keyword == COMMA) {
				getSym();
			}
			else {}
			oneDimensionalAssignment(identifier,flag, arraySizeOfJ);
			count++;
		}
	}
	if (count != arraySizeOfI) {
		errorPrint('n', mylexicalanalyzer.preLine);
	}
	if (keyword == RBRACE) {
		getSym();
	}
	else {}
}

void grammarAnalyzer::addArray(bool flag, std::string identifier,int sizei,int sizej) {
	if (ST::lookUp(currentFunc, identifier, true) == nullptr) {
		if (flag) {
			ST::addSymbol(currentFunc, identifier, INT, ARRAY, sizei, sizej, mylexicalanalyzer.preLine);
		}
		else {
			ST::addSymbol(currentFunc, identifier, CHAR, ARRAY, sizei, sizej, mylexicalanalyzer.preLine);
		}
	}
	else {
		errorPrint('b', mylexicalanalyzer.preLine);
	}
}

void  grammarAnalyzer::addVar(bool flag, std::string identifier,int value) {
	if (ST::lookUp(currentFunc, identifier, true) == nullptr) {
		if (flag) {
			ST::addSymbol(currentFunc, identifier, INT, VAR, value, 0,mylexicalanalyzer.preLine);
		}
		else {
			ST::addSymbol(currentFunc, identifier, CHAR, VAR, value, 0,mylexicalanalyzer.preLine);
		}
	}
	else {
		errorPrint('b', mylexicalanalyzer.preLine);
	}
}

void grammarAnalyzer::varDefinition(bool flag,std::string identifier) {
	if (keyword == LBRACK) {
		oneDimensionalArray();
		if (keyword == LBRACK) {
			twoDimensionalArray();
			addArray(flag, identifier, arraySizei, arraySizej);
			if (keyword == ASSIGN) {
				getSym();
				twoDimensionalAssignment(identifier,flag, arraySizei, arraySizej);
				printGrammarAnswer("<变量定义及初始化>")
			}
			else if (keyword == COMMA) {
				getSym();
				varDefinitionWithoutInitial(flag);
			}
			else if (keyword == SEMICN) {
				printGrammarAnswer("<变量定义无初始化>")
			}
		}
		else if (keyword == ASSIGN) {
			addArray(flag, identifier, arraySizei,0);
			getSym();
			oneDimensionalAssignment(identifier,flag, arraySizei);
			printGrammarAnswer("<变量定义及初始化>")
		}
		else if (keyword == COMMA) {
			addArray(flag, identifier, arraySizei,0);
			getSym();
			varDefinitionWithoutInitial(flag);
		}
		else if (keyword == SEMICN) {
			addArray(flag, identifier, arraySizei,0);
			printGrammarAnswer("<变量定义无初始化>")
		}
		else {}
	}
	else if (keyword == COMMA) {
		addVar(flag, identifier,0);
		getSym();
		varDefinitionWithoutInitial(flag);
	}
	else if (keyword == SEMICN) {
		addVar(flag, identifier,0);
		printGrammarAnswer("<变量定义无初始化>")
	}
	else if (keyword == ASSIGN) {
		getSym();
		if (flag) {
			std::string value;
			int numValue=integer(value);
			addVar(flag, identifier, numValue);
			printGrammarAnswer("<常量>")
		}
		else {
			if (keyword == CHARCON) {
				addVar(flag, identifier, mylexicalanalyzer.intValue);
				getSym();
				printGrammarAnswer("<常量>")
			}
			else { 
				errorPrint('o', mylexicalanalyzer.currentLine); 
				getSym();
			}
		}
		printGrammarAnswer("<变量定义及初始化>")
	}
	else if (keyword == LPARENT) {
		return;//读到函数定义开始回退
	}
	printGrammarAnswer("<变量定义>")
}

void grammarAnalyzer::varDefinitionWithoutInitial(bool flag) {
	std::string identifierTemp;
	while (true) {
		if (keyword == SEMICN) {
			break;
		}
		if (keyword == IDENFR) {
			identifierTemp = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		else {}
		if (keyword == LBRACK) {
			oneDimensionalArray();
			if (keyword == LBRACK) {
				twoDimensionalArray();
				addArray(flag, identifierTemp, arraySizei,arraySizej);
				if (keyword == COMMA) {
					getSym();
					continue;
				}
			}
			else if (keyword == COMMA) {
				addArray(flag, identifierTemp, arraySizei,0);
				getSym();
				continue;
			}
			else if (keyword == SEMICN) {
				addArray(flag, identifierTemp, arraySizei, 0);
			}
		}
		else if (keyword == COMMA) {
			addVar(flag, identifierTemp,0);
			getSym();
			continue;
		}
		else if(keyword==SEMICN){
			addVar(flag, identifierTemp,0);
		}
		else {
			return;
		}
	}
	printGrammarAnswer("<变量定义无初始化>")
}

/*
void grammarAnalyzer::varDefinitionInitial(bool flag) {
	if (keyword == IDENFR) {
		getSym();
	}
	else {}
	if (keyword == LBRACK) {
		oneDimensionalArray();
		if (keyword == LBRACK) {
			twoDimensionalArray();
			if (keyword == ASSIGN) {
				getSym();
				twoDimensionalAssignment(identifier,flag, arraySizei, arraySizej);
			}
			else {}
		}
		else if (keyword == ASSIGN) {
			getSym();
			oneDimensionalAssignment(flag, arraySizei);
		}
		else if (keyword == SEMICN) {

		}
		else {}
	}
	else if (keyword == ASSIGN) {
		getSym();
		if (flag) {
			std::string value;
			integer(value);
		}
		else {
			if (keyword == CHARCON) {
				getSym();
			}
			else {
			}
		}
	}
	printGrammarAnswer("<变量定义及初始化>")
}
*/
void grammarAnalyzer::returnFuncDefinition(std::string funcName,int type) {
	currentFunc = "";
	hasReturn = false;
	if (ST::lookUp(currentFunc, funcName, true) == nullptr) {
		ST::addSymbol(currentFunc, funcName, type, FUNC, 0, 0,mylexicalanalyzer.preLine);
	}
	else {
		errorPrint('b', mylexicalanalyzer.preLine);
	}
	transform(funcName.begin(), funcName.end(), funcName.begin(), ::tolower);
	currentFunc = funcName;
	if (keyword == LPARENT) {
		getSym();
		parametersTable();
	}
	else {}
	if (keyword == RPARENT) {
		getSym();
	}
	else { errorPrint('l', mylexicalanalyzer.currentLine); }
	MC::addIMC(funcName, OP::FUNC_BEGIN, "0", "0");
	if (keyword == LBRACE) {
		getSym();
		compoundSentence();
	}
	else {}
	if (keyword == RBRACE) {
		if (hasReturn == false) {
			errorPrint('h', mylexicalanalyzer.currentLine);
		}
		getSym();
	}
	MC::addIMC("0", OP::FUNC_END, "0", "0");
	printGrammarAnswer("<有返回值函数定义>")
}

void grammarAnalyzer::parametersTable() {
	std::string temp;
	bool flag;
	int count = 0;
	if (keyword == RPARENT) {
	}
	else if (keyword == INTTK || keyword == CHARTK) {
		if (keyword == INTTK) {
			flag = true;
		}
		else {
			flag = false;
		}
		getSym();
		if (keyword == IDENFR) {
			temp = *mylexicalanalyzer.stringPointer;
			if (ST::lookUp(currentFunc, temp, true) == nullptr) {
				if (flag) {
					ST::addSymbol(currentFunc, temp, INT, PARA, count++,0,mylexicalanalyzer.preLine);
				}
				else {
					ST::addSymbol(currentFunc, temp, CHAR, PARA, count++,0, mylexicalanalyzer.preLine);
				}
			}
			else {
				errorPrint('b', mylexicalanalyzer.currentLine);
			}
			getSym();
		}
		else {}
		while (keyword == COMMA) {
			getSym();
			if (keyword == INTTK || keyword == CHARTK) {
				if (keyword == INTTK) {
					flag = true;
				}
				else {
					flag = false;
				}
				getSym();
			}
			else {}
			if (keyword == IDENFR) {
				temp = *mylexicalanalyzer.stringPointer;
				if (ST::lookUp(currentFunc, temp, true) == nullptr) {
					if (flag) {
						ST::addSymbol(currentFunc, temp, INT, PARA, count++, 0,mylexicalanalyzer.preLine);
					}
					else {
						ST::addSymbol(currentFunc, temp, CHAR, PARA, count++, 0,mylexicalanalyzer.preLine);
					}
				}
				else {
					errorPrint('b', mylexicalanalyzer.currentLine);
				}
				getSym();
			}
			else {}
		}
	}
	else { }
	printGrammarAnswer("<参数表>")
}

void grammarAnalyzer::noReturnFuncDefinition(std::string funcName) {
	currentFunc = "";
	hasReturn = false;
	if (ST::lookUp(currentFunc, funcName, true) == nullptr) {
		ST::addSymbol(currentFunc, funcName, VOID, FUNC, 0, 0,mylexicalanalyzer.preLine);
	}
	else {
		errorPrint('b', mylexicalanalyzer.preLine);
	}
	transform(funcName.begin(), funcName.end(), funcName.begin(), ::tolower);
	currentFunc = funcName;
	if (keyword == LPARENT) {
		getSym();
		parametersTable();
	}
	else {}
	if (keyword == RPARENT) {
		getSym();
	}
	else { errorPrint('l', mylexicalanalyzer.currentLine); }
	MC::addIMC(funcName, OP::FUNC_BEGIN, "0", "0");
	if (keyword == LBRACE) {
		getSym();
		compoundSentence();
	}
	else {}
	if (keyword == RBRACE) {
		getSym();
	}
	MC::addIMC("0", OP::FUNC_END, "0", "0");
	printGrammarAnswer("<无返回值函数定义>")
}

void grammarAnalyzer::compoundSentence() {
	std::string identifierTemp;
	if (keyword == CONSTTK) {
		constStatement();
	}
	bool flag;
	if (keyword == INTTK) {
		flag = true;
		getSym();
		if (keyword == IDENFR) {
			identifierTemp = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		varStatement(flag,identifierTemp);
	}
	else if (keyword == CHARTK) {
		flag = false;
		getSym();
		if (keyword == IDENFR) {
			identifierTemp = *mylexicalanalyzer.stringPointer;
			getSym();
		}
		varStatement(flag,identifierTemp);
	}
	statementList();
	printGrammarAnswer("<复合语句>")
}


void grammarAnalyzer::statementList() {
	while (keyword != RBRACE) {
		statement();
	}
	printGrammarAnswer("<语句列>")
}

void grammarAnalyzer::statement() {
	if (keyword == WHILETK || keyword == FORTK) {
		loopStatement();
	}
	else if (keyword == IFTK) {
		getSym();
		conditionalStatements();
	}
	else if (keyword == IDENFR) {
		std::string name=*mylexicalanalyzer.stringPointer;
		bool flag;
		auto iter = funcIsReturnMap.find(*mylexicalanalyzer.stringPointer);
		if (iter != funcIsReturnMap.end()) {
			flag = iter->second;
		}
		else {}
		getSym();
		if (keyword == ASSIGN || keyword == LBRACK) {
			int kind;
			if (ST::lookUp(currentFunc, name, true) != nullptr) {
				kind = ST::lookUp(currentFunc, name, true)->kind;
			}
			else if (ST::lookUp("", name, true) != nullptr) {
				kind = ST::lookUp("", name, true)->kind;
			}
			else {
				errorPrint('c', mylexicalanalyzer.preLine);
			}
			if (kind == CONST) {
				errorPrint('j', mylexicalanalyzer.preLine);
			}
			assignmentStatement(name);
			if (keyword == SEMICN) {
				getSym();
			}else{ errorPrint('k', mylexicalanalyzer.preLine); }
		}
		else if (keyword == LPARENT) {
			int kind;
			if (ST::lookUp("", name, true) != nullptr) {
				kind = ST::lookUp("", name, true)->kind;
			}
			else {
				errorPrint('c', mylexicalanalyzer.preLine);
			}
			getSym();
			functionCallStatement(flag,name);
			if (keyword == SEMICN) {
				getSym();
			}else{ errorPrint('k', mylexicalanalyzer.preLine); }
		}
		else {}
	}
	else if (keyword == SCANFTK) {
		getSym();
		scanfSentence();
		if (keyword == SEMICN) {
			getSym();
		}else{ errorPrint('k', mylexicalanalyzer.preLine); }
	}
	else if (keyword == PRINTFTK) {
		getSym();
		printfSentence();
		if (keyword == SEMICN) {
			getSym();
		}else{ errorPrint('k', mylexicalanalyzer.preLine); }
	}
	else if (keyword == SWITCHTK) {
		getSym();
		switchStatements();
	}
	else if (keyword == SEMICN) {
		getSym();
	}
	else if (keyword == RETURNTK) {
		hasReturn = true;
		getSym();
		if (keyword == SEMICN && (globalSymbolTable[currentFunc].type != VOID)) {
			errorPrint('h', mylexicalanalyzer.currentLine);
		}
		returnStatement();
		if (keyword == SEMICN) {
			getSym();
		}else{ errorPrint('k', mylexicalanalyzer.preLine); }
	}
	else if (keyword == LBRACE) {
		getSym();
		statementList();
		if (keyword == RBRACE) {
			getSym();
		}
	}
	else {}
	printGrammarAnswer("<语句>")
}

void grammarAnalyzer::switchTable(int type,std::string labelOut,std::string value) {
	while (keyword == CASETK) {
		switchCaseStatements(type,labelOut,value);
	}
	printGrammarAnswer("<情况表>")
}

void grammarAnalyzer::switchCaseStatements(int type,std::string labelOut,std::string value) {
	std::string valueTemp;
	if (keyword == CASETK) {
		getSym();
		if ((keyword == PLUS || keyword == MINU || keyword == INTCON)&&type==INT) {
			integer(valueTemp);
		}
		else if (keyword == CHARCON&&type==CHAR) {
			std::string temp = "'";
			valueTemp = temp + ((*mylexicalanalyzer.stringPointer)[0]) + temp;
			getSym();
		}
		else {
			errorPrint('o', mylexicalanalyzer.currentLine);
			getSym();
		}
		std::string labelBne = MC::genLabel();
		MC::addIMC(labelBne, OP::NEQ, value, valueTemp);
		printGrammarAnswer("<常量>")
			if (keyword == COLON) {
				getSym();
				statement();
			}
		MC::addIMC(labelOut, OP::GOTO, "0", "0");
		MC::addIMC(labelBne, OP::LABEL, "0", "0");
	}
	printGrammarAnswer("<情况子语句>")
}

void grammarAnalyzer::defaultStatement() {
	if (keyword == DEFAULTTK) {
		getSym();
		if (keyword == COLON) {
			getSym();
			statement();
		}
	}
	else {
		errorPrint('p', mylexicalanalyzer.currentLine);
	}
	printGrammarAnswer("<缺省>")
}

void grammarAnalyzer::switchStatements() {
	if (keyword == LPARENT) {
		getSym();
		std::string value;
		std::string labelOut = MC::genLabel();
		int type=expression(value);
		if (type != 0) {
			type = INT;
		}
		else {
			type = CHAR;
		}
		if (keyword != RPARENT) {
			errorPrint('l', mylexicalanalyzer.currentLine);
		}else{ getSym(); }
		if (keyword == LBRACE) {
			getSym();
			switchTable(type,labelOut,value);
			defaultStatement();
			if (keyword == RBRACE) {
				getSym();
			}
		}
		MC::addIMC(labelOut, OP::LABEL, "0", "0");
	}
	printGrammarAnswer("<情况语句>")
}

void grammarAnalyzer::returnStatement() {
	if (keyword == LPARENT) {
		if (globalSymbolTable[currentFunc].type == VOID) {
			errorPrint('g', mylexicalanalyzer.currentLine);
		}
		getSym();
		if (keyword == RPARENT && (globalSymbolTable[currentFunc].type != VOID)) {
			errorPrint('h', mylexicalanalyzer.currentLine);
		}
		std::string value;
		int answer=expression(value);
		if ((answer != 0 && globalSymbolTable[currentFunc].type == CHAR) || (answer == 0 && globalSymbolTable[currentFunc].type == INT)) {
			errorPrint('h', mylexicalanalyzer.preLine);
		}
		
		if(value[0] == '#') {
			std::string t = MC::middleCode[MC::middleCode.size() - 1].rst;
			MC::middleCode[MC::middleCode.size() - 1].rst = "RET_v0";
			funcSymbolTable[currentFunc].erase(t);
			MC::temp_order--;
		}
		else {
			MC::addIMC("RET_v0", OP::ADD, value, "0");
		}
		if (keyword == RPARENT) {
			getSym();
		}
		else { errorPrint('l', mylexicalanalyzer.currentLine); }
	}
	if (currentFunc != "main") {
		MC::addIMC("0",OP::RET,"0","0");
	}
	else {
		MC::addIMC("0", OP::EXIT, "0", "0");
	}
	printGrammarAnswer("<返回语句>")
}

void grammarAnalyzer::printfSentence() {
	if (keyword == LPARENT) {
		getSym();
		if (keyword == STRCON) {
			int strNum = ST::addStr(*mylexicalanalyzer.stringPointer);
			MC::addIMC("str", OP::PRINT, MC::strhd + std::to_string(strNum), "0");
			getSym();
			printGrammarAnswer("<字符串>")
				if (keyword == COMMA) {
					getSym();
					std::string value;
					int ans = expression(value);
					if (ans == 0) {
						MC::addIMC("char", OP::PRINT, value, "0");
					}
					else {
						MC::addIMC("int", OP::PRINT, value, "0");
					}
					MC::addIMC("\n", OP::PRINT, "0", "0");
				}
				else {
					MC::addIMC("\n", OP::PRINT, "0", "0");
				}
		}
		else {
			std::string value;
			int ans=expression(value);
			std::cout << value << std::endl;
			if (ans == 0) {
				MC::addIMC("char", OP::PRINT, value, "0");
			}
			else {
				MC::addIMC("int", OP::PRINT, value, "0");
			}
			MC::addIMC("\n", OP::PRINT, "0", "0");
		}
	}
	else {}
	if (keyword == RPARENT) {
		getSym();
	}
	else { errorPrint('l', mylexicalanalyzer.currentLine); }
	printGrammarAnswer("<写语句>")
}

int grammarAnalyzer::checkInMap(std::string name) {
	int kind;
	if (ST::lookUp(currentFunc, name, true) != nullptr) {
		kind = ST::lookUp(currentFunc, name, true)->kind;
	}
	else if (ST::lookUp("", name, true) != nullptr) {
		kind = ST::lookUp("", name, true)->kind;
	}
	else {
		errorPrint('c', mylexicalanalyzer.preLine);
	}
	return kind;
}

void grammarAnalyzer::scanfSentence() {
	if (keyword == LPARENT) {
		getSym();
		if (keyword == IDENFR) {
			std::string name = *mylexicalanalyzer.stringPointer;
			int kind;
			int type;
			if (ST::lookUp(currentFunc, name, true) != nullptr) {
				kind = ST::lookUp(currentFunc, name, true)->kind;
				type = ST::lookUp(currentFunc, name, true)->type;
			}
			else if (ST::lookUp("", name, true) != nullptr) {
				kind = ST::lookUp("", name, true)->kind;
				type = ST::lookUp("", name, true)->type;
			}
			if (kind == CONST) {
				errorPrint('j', mylexicalanalyzer.currentLine);
			}
			getSym();
			if (keyword == RPARENT) {
				getSym();
			}
			else { errorPrint('l', mylexicalanalyzer.currentLine); }
			if (type == INT) {
				MC::addIMC("int", OP::SCAN, name, "0");
			}
			else if(type==CHAR){
				MC::addIMC("char", OP::SCAN, name, "0");
			}
		}
	}
	else {}
	printGrammarAnswer("<读语句>")
}


void grammarAnalyzer::loopStatement() {
	if (keyword == WHILETK) {
		std::string labelTemp1 = MC::genLabel();
		MC::addIMC(labelTemp1, OP::LABEL, "0", "0");
		std::string labelTemp2 = MC::genLabel();
		getSym();
		if (keyword == LPARENT) {
			getSym();
			condition(labelTemp2);
			if (keyword != RPARENT) {
				errorPrint('l', mylexicalanalyzer.preLine);
			}else{ getSym(); }
			statement();
			MC::addIMC(labelTemp1, OP::GOTO, "0", "0");
			MC::addIMC(labelTemp2, OP::LABEL, "0", "0");
		}
	}
	else if (keyword == FORTK) {
		getSym();
		if (keyword == LPARENT) {
			getSym();
			if (keyword == IDENFR) {
				std::string name = *mylexicalanalyzer.stringPointer;
				checkInMap(name);
				getSym();
				if (keyword == ASSIGN) {
					getSym();
					std::string value;
					expression(value);
					if (value[0] == '#') {
						std::string t = MC::middleCode[MC::middleCode.size() - 1].rst;
						MC::middleCode[MC::middleCode.size() - 1].rst = name;
						funcSymbolTable[currentFunc].erase(t);
						MC::temp_order--;
					}
					else {
						MC::addIMC(name, OP::ADD, value, "0");
					}
					if (keyword != SEMICN) {
						errorPrint('k', mylexicalanalyzer.preLine);
					}
					else {
						getSym();
					}
					std::string forLabel = MC::genLabel();
					std::string outLabel = MC::genLabel();

					MC::addIMC(forLabel, OP::LABEL, "0", "0");
					
					std::string value1;
					expression(value1);
					keyWord tempKey = keyword;
					if (keyword == LSS || keyword == LEQ || keyword == GRE || keyword == GEQ || keyword == EQL || keyword == NEQ) {
						getSym();
					}
					std::string value2;
					expression(value2);

					MC::addIMC(outLabel, (tempKey == NEQ ? OP::EQU :
						tempKey == EQL ? OP::NEQ :
						tempKey == GRE ? OP::LESEQ :
						tempKey == GEQ ? OP::LES :
						tempKey == LSS ? OP::GREQ : OP::GRT), value1, value2);

					if (keyword != SEMICN) {
						errorPrint('k', mylexicalanalyzer.preLine);
					}
					else {
						getSym();
					}
					if (keyword == IDENFR) {
						std::string name1 = *mylexicalanalyzer.stringPointer;
						checkInMap(name1);
						getSym();
						if (keyword == ASSIGN) {
							getSym();
							if (keyword == IDENFR) {
								std::string name2 = *mylexicalanalyzer.stringPointer;
								checkInMap(name2);
								getSym();
								keyWord tempKeyOp = keyword;
								if (keyword == PLUS || keyword == MINU) {
									getSym();
									unsigned int tempNum=step();
									if (keyword != RPARENT) {
										errorPrint('l', mylexicalanalyzer.preLine);
									}else{ getSym(); }
									statement();	
									MC::addIMC(name1, (tempKeyOp == PLUS ? OP::ADD : OP::SUB), name2, std::to_string(tempNum));
									MC::addIMC(forLabel, (tempKey == NEQ ? OP::NEQ :
										tempKey == EQL ? OP::EQU :
										tempKey == GRE ? OP::GRT :
										tempKey == GEQ ? OP::GREQ :
										tempKey == LSS ? OP::LES : OP::LESEQ), value1, value2);
									MC::addIMC(outLabel, OP::LABEL, "0", "0");
								}
							}
						}
					}
				}
			}
		}
	}
	printGrammarAnswer("<循环语句>")
}

int grammarAnalyzer::step() {
	std::string value;
	int ans=unsignedInteger(value);
	printGrammarAnswer("<步长>")
		return ans;
}

void grammarAnalyzer::condition(std::string label) {
	int judge = 0;
	std::string value1;
	int answer=expression(value1);
	if (answer == 0&&judge==0) {
		errorPrint('f', mylexicalanalyzer.preLine);
		judge = 1;
	}
	keyWord tempKey = keyword;
	if (keyword == LSS || keyword == LEQ || keyword == GRE || keyword == GEQ || keyword == EQL || keyword == NEQ) {
		getSym();
	}
	std::string value2;
	answer=expression(value2);
	if (answer == 0&&judge==0) {
		errorPrint('f', mylexicalanalyzer.preLine);
		judge = 1;
	}
	if (tempKey == EQL) {
		MC::addIMC(label, OP::NEQ, value1, value2);
	}
	else if (tempKey == NEQ) {
		MC::addIMC(label, OP::EQU, value1, value2);
	}
	else if (tempKey == LEQ) {
		MC::addIMC(label, OP::GRT, value1, value2);
	}
	else if (tempKey == LSS) {
		MC::addIMC(label, OP::GREQ, value1, value2);
	}
	else if (tempKey == GEQ) {
		MC::addIMC(label, OP::LES, value1, value2);
	}
	else if (tempKey == GRE) {
		MC::addIMC(label, OP::LESEQ, value1, value2);
	}
	printGrammarAnswer("<条件>")
}

void grammarAnalyzer::conditionalStatements() {
	if (keyword == LPARENT) {
		getSym();
		std::string labelTemp1 = MC::genLabel();
		condition(labelTemp1);
		if (keyword != RPARENT) {
			errorPrint('l', mylexicalanalyzer.preLine);
		}else{ getSym(); }
		statement();
		if (keyword == ELSETK) {
			std::string labelTemp2 = MC::genLabel();
			MC::addIMC(labelTemp2, OP::GOTO, "0", "0");
			MC::addIMC(labelTemp1, OP::LABEL, "0", "0");
			getSym();
			statement();
			MC::addIMC(labelTemp2, OP::LABEL, "0", "0");
		}
		else {
			MC::addIMC(labelTemp1, OP::LABEL, "0", "0");
		}
	}
	printGrammarAnswer("<条件语句>")
}

void grammarAnalyzer::functionCallStatement(bool flag,std::string nameIn) {
	valueParameterTable(nameIn);
	MC::addIMC(nameIn, OP::CALL, "0", "0");
	if (keyword == RPARENT) {
		getSym();
	}
	else { errorPrint('l', mylexicalanalyzer.preLine); }
	if (flag) {
		printGrammarAnswer("<有返回值函数调用语句>")
	}
	else {
		printGrammarAnswer("<无返回值函数调用语句>")
	}
}

void grammarAnalyzer::valueParameterTable(std::string nameIn) {
	int count = -1;
	int judge = 0;
	std::string value;
	transform(nameIn.begin(), nameIn.end(), nameIn.begin(), ::tolower);
	if (keyword == RPARENT) {
	}
	else if (keyword == IDENFR || keyword == PLUS || keyword == MINU || keyword == INTCON || keyword == CHARCON || keyword == LPARENT) {
		count++;
		int type = 0;
		type= ST::lookUpPara(funcSymbolTable[nameIn], count);
		int answer=expression(value);
		MC::addIMC(nameIn, OP::PUSH, value, std::to_string(count));
		if ((answer == 0 && type == INT) || (answer != 0 && type == CHAR)&&judge==0) {
			judge = 1;
			errorPrint('e', mylexicalanalyzer.preLine);
		}
		while (keyword == COMMA) {
			count++;
			getSym();
			if (keyword == IDENFR || keyword == PLUS || keyword == MINU || keyword == INTCON || keyword == CHARCON || keyword == LPARENT) {
				int type=0;
				type = ST::lookUpPara(funcSymbolTable[nameIn], count);
				int answer = expression(value);
				MC::addIMC(nameIn, OP::PUSH, value, std::to_string(count));
				if ((answer == 0 && type == INT)||(answer != 0 && type == CHAR)&&judge==0) {
					judge = 1;
					errorPrint('e', mylexicalanalyzer.preLine);
				}
			}
			else {}
		}
	}
	else { }
	if (count != globalSymbolTable[nameIn].length) {
		errorPrint('d', mylexicalanalyzer.preLine);
	}
	printGrammarAnswer("<值参数表>")
}

int grammarAnalyzer::expression(std::string &value) {
	int answer = 0;
	int symbol = 1;
	if (keyword == PLUS || keyword == MINU) {
		symbol = keyword == PLUS ? 1 : -1;
		answer++;
		getSym();
	}
	std::string term1;
	answer+=item(term1);
	if (symbol == -1) {
		std::string minus_trem = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
		MC::addIMC(minus_trem, OP::SUB, "0", term1);
		term1 = minus_trem;
	}
	while (keyword == PLUS || keyword == MINU) {
		std::string term2;
		symbol = keyword == PLUS ? 1 : -1;
		answer++;
		getSym();
		item(term2);
		std::string term3 = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
		MC::addIMC(term3, symbol == 1 ? OP::ADD:OP::SUB, term1, term2);
		term1 = term3;
	}
	value = term1;
	printGrammarAnswer("<表达式>")
		return answer;
}

int grammarAnalyzer::item(std::string &value) {
	std::string factor1;
	int answer = 0;
	answer+=factor(factor1);
	while (keyword == MULT || keyword == DIV) {
		int symbol = keyword == MULT ? 1 : -1;
		std::string factor2;
		answer++;
		getSym();
		factor(factor2);
		std::string factor3 = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
		MC::addIMC(factor3, symbol == 1 ? OP::MUL : OP::DIV, factor1, factor2);
		factor1 = factor3;
	}
	value = factor1;
	printGrammarAnswer("<项>")
		return answer;
}

int grammarAnalyzer::factor(std::string &value) {
	int answer = 0;
	std::string name;
	if (keyword == IDENFR) {
		name = *mylexicalanalyzer.stringPointer;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		getSym();
		if (keyword == LBRACK) {
			int temp;
			int len;
			if (ST::lookUp(currentFunc, name, true) != nullptr) {
				temp = ST::lookUp(currentFunc, name, true)->type;
				len = ST::lookUp(currentFunc, name, true)->length1;
			}else if (ST::lookUp("", name, true) != nullptr) {
				temp = ST::lookUp("", name, true)->type;
				len = ST::lookUp("", name, true)->length1;
			}
			else {
				errorPrint('c', mylexicalanalyzer.preLine);
			}
			if (temp == INT) {
				answer++;
			}
			getSym();
			std::string value1;
			int tempExpression = expression(value1);
			if (tempExpression == 0) {
				errorPrint('i', mylexicalanalyzer.preLine);
			}
			if (keyword != RBRACK) {
				errorPrint('m', mylexicalanalyzer.preLine);
			}else{ getSym(); }
			if (keyword == LBRACK) {
				getSym();
				std::string value2;
				int tempExpression = expression(value2);
				if (tempExpression == 0) {
					errorPrint('i', mylexicalanalyzer.preLine);
				}
				if (keyword == RBRACK) {
					getSym();
				}
				else { errorPrint('m', mylexicalanalyzer.preLine); }
				value = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, temp);
				std::string temp1= MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
				MC::addIMC(temp1, OP::MUL, value1, std::to_string(len));
				std::string temp2= MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
				MC::addIMC(temp2, OP::ADD, temp1, value2);
				MC::addIMC(value, OP::READARR, name, temp2);
			}
			else {
				value = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, temp);
				MC::addIMC(value, OP::READARR, name, value1);
			}
		}
		else if (keyword == LPARENT) {
			int temp = globalSymbolTable[name].type;
			if (temp == INT) {
				answer++;
			}
			getSym();
			//functionCallStatement(true,name);
			valueParameterTable(name);
			if (keyword == RPARENT) {
				getSym();
			}
			MC::addIMC(name, OP::CALL, "0", "0");
			std::string ret = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, globalSymbolTable[name].type);
			MC::addIMC(ret, OP::ADD, "RET_v0", "0");
			value = ret;
		}
		else {
			int temp;
			int kind;
			int v;
			if (ST::lookUp(currentFunc, name, true) != nullptr) {
				temp = ST::lookUp(currentFunc, name, true)->type;
				kind = ST::lookUp(currentFunc, name, true)->kind;
				if (kind == CONST) {
					v = ST::lookUp(currentFunc, name, true)->length;
				}
			}
			else if (ST::lookUp("", name, true) != nullptr) {
				temp = ST::lookUp("", name, true)->type;
				kind = ST::lookUp("", name, true)->kind;
				if (kind == CONST) {
					v = ST::lookUp("", name, true)->length;
				}
			}
			else {
				errorPrint('c', mylexicalanalyzer.preLine);
			}
			if (kind == VAR||kind==PARA) {
				value = name;
			}
			else if (kind == CONST) {
				value = std::to_string(v);
			}
			if (temp == INT) {
				answer++;
			}
		}
	}
	else if (keyword == LPARENT) {
		answer++;
		getSym();
		expression(value);
		if (keyword == RPARENT) {
			getSym();
		}
		else { errorPrint('l', mylexicalanalyzer.preLine); }
	}
	else if (keyword == INTCON || keyword == PLUS || keyword == MINU) {
		answer++;
		integer(value);
	}
	else if (keyword == CHARCON) {
		std::string temp = "'";
		value = temp + ((*mylexicalanalyzer.stringPointer)[0]) + temp;
		getSym();
		//value = std::to_string(mylexicalanalyzer.intValue);
	}
	else {}
	printGrammarAnswer("<因子>")
		return answer;
}

void grammarAnalyzer::assignmentStatement(std::string name) {
	std::string value;
	if (keyword == ASSIGN) {
		getSym();
		expression(value);
		if (value[0] == '#') {
			std::string t = MC::middleCode[MC::middleCode.size() - 1].rst;
			MC::middleCode[MC::middleCode.size() - 1].rst = name;
			funcSymbolTable[currentFunc].erase(t);
			MC::temp_order--;
		}
		else {
			MC::addIMC(name,OP::ADD,value,"0");
		}
	}
	else if (keyword == LBRACK) {
		getSym();
		std::string value1;
		int answer=expression(value1);
		if (answer == 0) {
			errorPrint('i', mylexicalanalyzer.preLine);
		}
		if (keyword != RBRACK) {
			errorPrint('m', mylexicalanalyzer.preLine);
		}else{ getSym(); }
		if (keyword == LBRACK) {
			getSym();
			std::string value2,value4;
			answer = expression(value2);
			if (answer == 0) {
				errorPrint('i', mylexicalanalyzer.preLine);
			}
			if (keyword != RBRACK) {
				errorPrint('m', mylexicalanalyzer.preLine);
			}
			else { getSym(); }
			if (keyword == ASSIGN) {
				getSym();
				expression(value4);
			}
			else {}
			int len;
			if (ST::lookUp(currentFunc, name, true) != nullptr) {
				len = ST::lookUp(currentFunc, name, true)->length1;
			}
			else {
				len = globalSymbolTable[name].length1;
			}
			std::string temp = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
			MC::addIMC(temp, OP::MUL, value1, std::to_string(len));
			std::string temp1 = MC::genTemp(currentFunc, mylexicalanalyzer.currentLine, INT);
			MC::addIMC(temp1, OP::ADD, temp, value2);
			MC::addIMC(name, OP::SAVEARR, temp1, value4);

		}
		else if (keyword == ASSIGN) {
			getSym();
			std::string value3;
			expression(value3);
			MC::addIMC(name, OP::SAVEARR, value1, value3);
		}
		else {}
	}
	printGrammarAnswer("<赋值语句>")
}
