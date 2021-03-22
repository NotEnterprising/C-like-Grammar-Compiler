#include <iostream>
#include <ctype.h>
#include <unordered_map>
#include <string>
#include "lexicalanalysis.h"
#include <algorithm>

static std::unordered_map<std::string, keyWord> reservedMap;

static char currentChar;
static std::string currentString;


lexicalanalysis::lexicalanalysis(std::ifstream& input, std::ofstream& output, std::vector<std::string>&vectorin,error& Error):fileInput(input), fileOutput(output),printvector(vectorin),myError(Error) {
	intValue = 0;
	keyword = ILLEGAL;
	preKeyWord = ILLEGAL;
	currentLine = 1;

	stringPointer = NULL;
	reservedMap.insert(std::pair<std::string, keyWord>("const", CONSTTK));
	reservedMap.insert(std::pair<std::string, keyWord>("int", INTTK));
	reservedMap.insert(std::pair<std::string, keyWord>("char", CHARTK));
	reservedMap.insert(std::pair<std::string, keyWord>("void", VOIDTK));
	reservedMap.insert(std::pair<std::string, keyWord>("main", MAINTK));
	reservedMap.insert(std::pair<std::string, keyWord>("if", IFTK));
	reservedMap.insert(std::pair<std::string, keyWord>("else", ELSETK));
	reservedMap.insert(std::pair<std::string, keyWord>("switch", SWITCHTK));
	reservedMap.insert(std::pair<std::string, keyWord>("case", CASETK));
	reservedMap.insert(std::pair<std::string, keyWord>("default", DEFAULTTK));
	reservedMap.insert(std::pair<std::string, keyWord>("while", WHILETK));
	reservedMap.insert(std::pair<std::string, keyWord>("for", FORTK));
	reservedMap.insert(std::pair<std::string, keyWord>("scanf", SCANFTK));
	reservedMap.insert(std::pair<std::string, keyWord>("printf", PRINTFTK));
	reservedMap.insert(std::pair<std::string, keyWord>("return", RETURNTK));
}


void lexicalanalysis::initial() {
	currentChar = fileInput.get();
}

void lexicalanalysis::getSym() {
	preLine = currentLine;
	currentString.clear();
	while (isspace(currentChar)) {
		if (currentChar == '\n') {
			currentLine++;
		}
		currentChar = fileInput.get();
	}
	if (isalpha(currentChar)||currentChar == '_') {
		stringPointer = &currentString;
		while (isalnum(currentChar)||currentChar=='_') {
			currentString.append(1, currentChar);
			currentChar = fileInput.get();
		}
		std::string str = currentString;
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		transform(currentString.begin(), currentString.end(), currentString.begin(), ::tolower);
		auto iter = reservedMap.find(str);
		if (iter != reservedMap.end()) {
			keyword = iter->second;
		}
		else {
			keyword = IDENFR;
		}
	}
	else if (isdigit(currentChar)) {
		intValue = 0;
		stringPointer = &currentString;
		while (isdigit(currentChar)) {
			intValue = intValue * 10 + currentChar - '0';
			currentString.append(1, currentChar);
			currentChar = fileInput.get();
		}
		keyword = INTCON;
	}
	else {
		currentString.clear();
		stringPointer = &currentString;
		currentString.append(1, currentChar);
		if (currentChar == '+') {
			keyword = PLUS;
		}else if(currentChar=='-'){
			keyword = MINU;
		}
		else if (currentChar == '*') {
			keyword = MULT;
		}
		else if (currentChar == '/') {
			keyword = DIV;
		}
		else if (currentChar == '<') {
			currentChar = fileInput.peek();
			if (currentChar == '=') {
				currentString.append(1, currentChar);
				keyword = LEQ;
				currentChar = fileInput.get();
			}
			else {
				keyword = LSS;
			}
		}
		else if (currentChar == '>') {
			currentChar = fileInput.peek();
			if (currentChar == '=') {
				currentString.append(1, currentChar);
				keyword = GEQ;
				currentChar = fileInput.get();
			}
			else {
				keyword = GRE;
			}
		}
		else if (currentChar == '=') {
			currentChar = fileInput.peek();
			if (currentChar == '=') {
				currentString.append(1, currentChar);
				keyword = EQL;
				currentChar = fileInput.get();
			}
			else {
				keyword = ASSIGN;
			}
		}
		else if (currentChar == '!') {
			currentChar = fileInput.peek();
			if (currentChar == '=') {
				currentString.append(1, currentChar);
				keyword = NEQ;
				currentChar = fileInput.get();
			}
			else {
				keyword = ILLEGAL;
			}
		}
		else if (currentChar == ':') {
			keyword = COLON;
		}
		else if (currentChar == ';') {
			keyword = SEMICN;
		}
		else if (currentChar == ',') {
			keyword = COMMA;
		}
		else if (currentChar == '(') {
			keyword = LPARENT;
		}
		else if (currentChar == ')') {
			keyword = RPARENT;
		}
		else if (currentChar == '[') {
			keyword = LBRACK;
		}
		else if (currentChar == ']') {
			keyword = RBRACK;
		}
		else if (currentChar == '{') {
			keyword = LBRACE;
		}
		else if (currentChar == '}') {
			keyword = RBRACE;
		}
		else if (currentChar == '\'') {
			currentChar = fileInput.peek();
			if (isalnum(currentChar) || currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/' || currentChar == '_') {
				currentChar = fileInput.get();
				currentString.clear();
				stringPointer = &currentString;
				intValue = currentChar;
				currentString.append(1, currentChar);
				currentChar = fileInput.peek();
				if (currentChar != '\'') {
					keyword = ILLEGAL;
				}
				currentChar = fileInput.get();
				keyword = CHARCON;
			}
			else if (currentChar == '\'') {
				currentChar = fileInput.get();
				myError.errorPrint('a', currentLine);
				keyword = CHARCON;
			}
			else {
				currentChar = fileInput.get();
				myError.errorPrint('a', currentLine);
				currentChar = fileInput.get();
				keyword = CHARCON;
			}
		}
		else if (currentChar == '\"') {
			int flag = 0;
			currentString.clear();
			stringPointer = &currentString;
			currentChar= fileInput.peek();
			if (currentChar!='\"') {
				while (currentChar != '\"') {
					if (!((currentChar >= 35 && currentChar <= 126) || currentChar == 32 || currentChar == 33) && flag == 0) {
						flag = 1;
						myError.errorPrint('a', currentLine);
					}
					currentChar = fileInput.get();
					if (currentChar == '\\'){
						currentString.append(1, currentChar);
					}
					currentString.append(1, currentChar);
					currentChar = fileInput.peek();
				}
				if (currentChar == '\"') {
					currentChar = fileInput.get();
					keyword = STRCON;
				}
			}
			else if(currentChar=='\"'){
				currentChar = fileInput.get();
				myError.errorPrint('a', currentLine);
				keyword = STRCON;
			}

		}
		else if (currentChar == EOF) {
			keyword = END;
		}
		else {
			keyword = ILLEGAL;
		}
		currentChar = fileInput.get();
	}
	if (preKeyWord != ILLEGAL && preKeyWord != END) {
		std::string temp = findEnum(preKeyWord) + " " + preString;
		printvector.emplace_back(temp);
	}
	preKeyWord = keyword;
	preString = currentString;
}

int lexicalanalysis::findTotalLine() {
	while (currentChar!=EOF) {
		if (currentChar == '\n') {
			currentLine++;
		}
		currentChar = fileInput.get();
	}
	return currentLine;
}

std::string lexicalanalysis::findEnum(keyWord keyword) {

	switch (keyword) {
	case 0:return "ILLEGAL";
	case 1:return "IDENFR";
	case 2:return "INTCON";
	case 3:return "CHARCON";
	case 4:return "STRCON";
	case 5:return "CONSTTK";
	case 6:return "INTTK";
	case 7:return "CHARTK";
	case 8:return "VOIDTK";
	case 9:return "MAINTK";
	case 10:return "IFTK";
	case 11:return "ELSETK";
	case 12:return "SWITCHTK";
	case 13:return "CASETK";
	case 14:return "DEFAULTTK";
	case 15:return "WHILETK";
	case 16:return "FORTK";
	case 17:return "SCANFTK";
	case 18:return "PRINTFTK";
	case 19:return "RETURNTK";
	case 20:return "PLUS";
	case 21:return "MINU";
	case 22:return "MULT";
	case 23:return "DIV";
	case 24:return "LSS";
	case 25:return "LEQ";
	case 26:return "GRE";
	case 27:return "GEQ";
	case 28:return "EQL";
	case 29:return "NEQ";
	case 30:return "COLON";
	case 31:return "ASSIGN";
	case 32:return "SEMICN";
	case 33:return "COMMA";
	case 34:return "LPARENT";
	case 35:return "RPARENT";
	case 36:return "LBRACK";
	case 37:return "RBRACK";
	case 38:return "LBRACE";
	case 39:return "RBRACE";
	case 40:return "END";
	}
}