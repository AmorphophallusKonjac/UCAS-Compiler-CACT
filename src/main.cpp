#include <iostream>
#include <ostream>

#include "IR/IRModule.h"
#include "FrontEnd.h"
#include "Optimizer.h"
#include "BackEnd.h"
#include "Interpreter/Interpreter.h"
#include "utils/CLParser.h"

using namespace antlr4;

int main(int argc, const char *argv[]) {
    CLParser parser;
    parser.add("simulate");
    parser.add("verbose");
    parser.add("c");
    parser.add("emit-IR");
    parser.parse(argc, argv);

    std::string file(parser.getFilePath());
    std::ifstream stream;
    stream.open(file);

    if (!stream.is_open()) {
        std::cerr << "Error: Fail to open " << file << std::endl;
        return 1;
    }

    IRModule ir(file);
    FrontEnd frontEnd(&stream, &ir);

    try {
        frontEnd.analyze();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    Optimizer opt(&ir);
    opt.build();
//    opt.setLevel(parser.getLevel());
    opt.setLevel(2);
    opt.run();

//    if (parser.get("emit-IR"))
    frontEnd.print();

    BackEnd backEnd(&ir);
    backEnd.print();

    Interpreter ip(&ir);
    Interpreter::debugOpt = parser.get("verbose");
    if (parser.get("simulate") || parser.get("verbose")) {
        int ret = ip.interpret();
        if (parser.get("c"))
            printf("inst_cnt = %d\n", Interpreter::getInstCnt());
        return ret;
    }

    return 0;
}