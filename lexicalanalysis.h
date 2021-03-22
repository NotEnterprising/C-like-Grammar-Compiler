#pragma once
#include <fstream>
#include <vector>
#include "error.h"
enum keyWord {
	ILLEGAL,
	IDENFR,
	INTCON,
	CHARCON,
	STRCON,
	CONSTTK,
	INTTK,
	CHARTK,
	VOIDTK,
	MAINTK,
	IFTK,
	ELSETK,
	SWITCHTK,
	CASETK,
	DEFAULTTK,
	WHILETK,
	FORTK,
	SCANFTK,
	PRINTFTK,
	RETURNTK,
	PLUS,
	MINU,
	MULT,
	DIV,
	LSS,
	LEQ,
	GRE,
	GEQ,
	EQL,
	NEQ,
	COLON,
	ASSIGN,
	SEMICN,
	COMMA,
	LPARENT,
	RPARENT,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	END
};

class lexicalanalysis {
public:
	lexicalanalysis(std::ifstream& input, std::ofstream& output, std::vector<std::string>& vectorin, error& Error);

	std::ifstream &fileInput;
	std::ofstream& fileOutput;
	std::vector<std::string> & printvector;
	keyWord keyword;

	int intValue;
	int currentLine;
	int preLine;

	error& myError;
	std::string* stringPointer;

	keyWord preKeyWord;
	std::string preString;

	void getSym();
	void initial();
	int  findTotalLine();
	std::string findEnum(keyWord keyword);
};
