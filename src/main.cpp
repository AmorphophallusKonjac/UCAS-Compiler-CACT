#include <iostream>
#include <ostream>

#include "SemanticAnalyzer.h"

using namespace antlr4;

int main(int argc, const char* argv[]) {
  	if (argc < 2) {
   	 	std::cerr << "Error: Missing source file" << std::endl;
		return 1;
	}
	std::ifstream stream;
	stream.open(argv[1]);

    if (!stream.is_open()) {
        std::cerr << "Error: Fail to open " << argv[1] << std::endl;
        return 1;
    }

    try {
        SemanticAnalyzer analyzer(&stream);
        analyzer.analyze();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

	return 0;
}