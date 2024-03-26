#ifndef COMPILER_ERRORHANDLER_H
#define COMPILER_ERRORHANDLER_H

#include "CACTVisitor.h"
#include "CACTLexer.h"
#include "CACTParser.h"
#include "CACTVisitor.h"
#include "tree/ParseTree.h"
//#include "symbolTable.h"

class SymbolInfo;

class ErrorHandler {
public:
    static void printErrorContext(antlr4::ParserRuleContext *context, const std::string& msg);
    static void printErrorSymbol(SymbolInfo *symbol, const std::string& msg);
    static void printErrorMessage(const std::string& msg);
};


#endif //COMPILER_ERRORHANDLER_H
