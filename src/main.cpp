#include <iostream>
#include <ostream>

#include "IR/IRModule.h"
#include "FrontEnd.h"
#include "Optimizer.h"

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
    //opt.build();

    //opt.run();

    frontEnd.print();

    return 0;
}