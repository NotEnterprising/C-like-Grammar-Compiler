#include "lexicalanalysis.h"
#include "grammarAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "genObjectCode.h"
int main() {
	std::ifstream filein("testfile.txt");

	std::ofstream fileout("output.txt");
	std::ofstream errorfile("error.txt");
	std::vector<std::string> printvector;
	error myError(errorfile);
	lexicalanalysis mylexicalanalyzer(filein,fileout,printvector,myError);
	grammarAnalyzer mygrammarAnalyzer(mylexicalanalyzer, fileout, printvector,myError);
	mygrammarAnalyzer.startAnalyze();
	GOC::middleCodeTOMips("mips.txt");
	/*
	for (int i = 0; i < printvector.size(); i++) {
		std::cout << printvector[i] << std::endl;
		fileout << printvector[i] << std::endl;
	}
	*/
	filein.close();
	fileout.close();
}


