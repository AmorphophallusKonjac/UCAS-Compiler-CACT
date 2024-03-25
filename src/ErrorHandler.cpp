#include "ErrorHandler.h"

void ErrorHandler::printErrorContext(antlr4::ParserRuleContext *context, const std::string& msg) {
    size_t line = context->start->getLine();
    size_t column = context->start->getCharPositionInLine();
    std::cerr << "Line " << std::to_string(line) << ":" << std::to_string(column) << " '" << context->getText() << "' " << msg << std::endl;
}
