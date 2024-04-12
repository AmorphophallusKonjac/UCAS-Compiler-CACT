#include "FrontEnd.h"
FrontEnd::FrontEnd(std::ifstream *stream, IRModule *ir)
    : input(*stream), lexer(&input), tokens(&lexer), parser(&tokens), ir(ir), root(parser.compilationUnit()), analyzer(&globalBlock, ir, root), generator(&globalBlock, ir, root) {
    globalBlock.initIOFunction();
}
void FrontEnd::analyze() {
    if (this->parser.getNumberOfSyntaxErrors() > 0 ||
        this->lexer.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "lexer error: " << lexer.getNumberOfSyntaxErrors()
                  << std::endl;
        std::cerr << "parser error: " << parser.getNumberOfSyntaxErrors()
                  << std::endl;
        throw std::runtime_error("Syntax analysis failed at " +
                                 std::string(__FILE__) + ":" +
                                 std::to_string(__LINE__));
    }

    //! 进行第一遍 visit, 进行全局语义检查, 添加全局变量、函数、局部变量在栈上的分配
    analyzer.analyze();
    //! 进行第二遍 visit, 添加各个指令
    generator.generate();
}
