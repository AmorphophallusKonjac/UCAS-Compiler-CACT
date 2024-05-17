#include <iostream>
#include <ostream>

#include "IR/IRModule.h"
#include "FrontEnd.h"
#include "Optimizer.h"
#include "Interpreter/Interpreter.h"

using namespace antlr4;

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Missing source file" << std::endl;
        return 1;
    }
    std::string file(argv[1]);
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

    // 在 build 方法中 new 自己定义的 pass 并添加进 opt
    opt.build();

    opt.run();

    frontEnd.print();

    if (argc > 2) {
        bool isInterpret = false;
        bool isDebug = false;
        bool isCount = false;
        for (int i = 2; i < argc; ++i) {
            std::string arg_i(argv[i]);
            if (arg_i == "-simulate") {
                isInterpret = true;
            }
            if (arg_i == "-verbose") {
                isInterpret = true;
                isDebug = true;
            }
            if (arg_i == "-c")
                isCount = true;
        }
        if (isInterpret) {
            if (isDebug) {
                Interpreter::debugOpt = true;
            } else {
                Interpreter::debugOpt = false;
            }
            Interpreter ip(&ir);
            int ret = ip.interpret();
            if (isCount)
                printf("inst_cnt = %d\n", Interpreter::getInstCnt());
            return ret;
        }
    }

    return 0;
}