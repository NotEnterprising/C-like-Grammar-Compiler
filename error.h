#pragma once
#include <fstream>

class error {
public:
	error(std::ofstream& output);

	std::ofstream& fileOutput;
	void errorPrint(char flag, int currentLine);
};
