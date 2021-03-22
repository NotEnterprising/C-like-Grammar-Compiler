#include "error.h"

error::error(std::ofstream& output):fileOutput(output){
	
}


void error::errorPrint(char flag,int currentLine) {
	fileOutput << currentLine << " " << flag << std::endl;
}