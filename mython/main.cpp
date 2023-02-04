#include "lexer.h"
#include "runtime.h"
#include "user_consol_interface.h"

#include <iostream>

using namespace std;

namespace {
void RunMythonProgram(istream &input, ostream &output) {
	parse::Lexer lexer(input);
	auto program = ParseProgram(lexer);

	runtime::SimpleContext context { output };
	runtime::Closure closure;
	program->Execute(closure, context);
}
}  // namespace

int main(int argc, char* argv[]) {
	//Обработка параметов переданных в программу
	UserRequest ur(argc,argv);
	try {
		RunMythonProgram(cin, cout);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
