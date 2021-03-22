#pragma once

#include <fstream>
#include "lexicalanalysis.h"
#include "error.h"
#include "intermediateCode.h"

#define printGrammarAnswer(str) { printvector.emplace_back(str); }
#define keyword mylexicalanalyzer.keyword 
#define getSym() mylexicalanalyzer.getSym();
class grammarAnalyzer {
public:
	grammarAnalyzer(lexicalanalysis& lexicalanalyzer, std::ofstream& output, std::vector<std::string>& vectorin,error& Error);

	error& myerror;

	lexicalanalysis& mylexicalanalyzer;

	std::ofstream& fileOutput;
	std::vector<std::string>& printvector;
	std::string funcName;
	int arraySizei ;
	int arraySizej ;
	std::string currentFunc;
	bool hasReturn;

	void startAnalyze();
	
	int checkInMap(std::string name);
	void errorPrint(char a,int line);
	void program();
	void constStatement();
	void addArray(bool flag, std::string identifier,int sizei,int sizej);
	void addVar(bool flag, std::string identifier,int value);
	int varStatement(bool flag, std::string identifier);
	void varDefinition(bool flag,std::string identifier);
	//void varDefinitionInitial(bool flag);
	void varDefinitionWithoutInitial(bool flag);
	void returnFuncDefinition(std::string funcName,int type);
	void noReturnFuncDefinition(std::string funcName);
	void constDefinition();
	int integer(std::string &value);
	void parametersTable();
	void compoundSentence();
	void statementList();
	void statement();
	void loopStatement();
	void conditionalStatements();
	void functionCallStatement(bool flag, std::string nameIn);
	void valueParameterTable(std::string nameIn);
	int expression(std::string& value);
	int item(std::string & value);
	int factor(std::string &value);
	void assignmentStatement(std::string name);
	void scanfSentence();
	void printfSentence();
	void returnStatement();
	int unsignedInteger(std::string &value);
	void condition(std::string label);
	int step();
	void switchStatements();
	void switchTable(int type,std::string labelOut,std::string value);
	void switchCaseStatements(int type,std::string labelOut,std::string value);
	void defaultStatement();
	void twoDimensionalArray();
	void oneDimensionalArray();
	void oneDimensionalAssignment(std::string identifier,bool flag,int arraySize);
	void twoDimensionalAssignment(std::string identifier,bool flag, int arraySizeOfI, int arraySizeOfJ);
};
