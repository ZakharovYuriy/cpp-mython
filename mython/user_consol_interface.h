#pragma once
#include <iostream>
#include <string>
#include "tests.h"

class UserRequest{
public:
UserRequest(int argc, char* argv[]){
using namespace std::literals;
	//обработка входных аргументов
	if (argc == 2) {
		const std::string mode(argv[1]);
		if (mode == "-help"s || mode == "-h"s) {
			PrintHelp();
			exit(EXIT_SUCCESS);
		} else if (mode == "-test"s || mode == "-t"s) {
			local_tests::TestAll();
			exit(EXIT_SUCCESS);
		} 
	}
}

private:

void PrintHelp(std::ostream &stream = std::cerr) {
	std::string help =
R"123(
ways to start mython:
-without parameters: after that, pass your program to stdin 
                     and press ctrl+D (pass the EOF symbol) 
                     after that, the program will output the 
                     response to stdout and terminate

-with -help or -h
 
-with -test or -t : run all the tests and return 
                     the result of passing them
)123";
	stream << help;
}

};
