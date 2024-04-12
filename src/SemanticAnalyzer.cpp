#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(std::ifstream *stream) : input(*stream), lexer(&input), tokens(&lexer), parser(&tokens) {
    root = this->parser.compilationUnit();

    if (this->parser.getNumberOfSyntaxErrors() > 0 || this->lexer.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "lexer error: " << lexer.getNumberOfSyntaxErrors() << std::endl;
        std::cerr << "parser error: " << parser.getNumberOfSyntaxErrors() << std::endl;
        throw std::runtime_error("Syntax analysis failed");
    }
    currentBlock = nullptr;
}

SemanticAnalyzer::~SemanticAnalyzer() = default;

std::any SemanticAnalyzer::visitFunctionType(CACTParser::FunctionTypeContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitBasicType(CACTParser::BasicTypeContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitPrimaryExpression(CACTParser::PrimaryExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitUnaryExpression(CACTParser::UnaryExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitMultiplicativeExpression(CACTParser::MultiplicativeExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitRelationalExpression(CACTParser::RelationalExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitEqualityExpression(CACTParser::EqualityExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitLogicalAndExpression(CACTParser::LogicalAndExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExpression(CACTParser::ExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitCondition(CACTParser::ConditionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitDeclaration(CACTParser::DeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantDeclaration(CACTParser::ConstantDeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantDefinition(CACTParser::ConstantDefinitionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantInitValue(CACTParser::ConstantInitValueContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitVariableDeclaration(CACTParser::VariableDeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitVariableDefinition(CACTParser::VariableDefinitionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitStatement(CACTParser::StatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitCompoundStatement(CACTParser::CompoundStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitBlockItemList(CACTParser::BlockItemListContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitBlockItem(CACTParser::BlockItemContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExpressionStatement(CACTParser::ExpressionStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitLValue(CACTParser::LValueContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitJumpStatement(CACTParser::JumpStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitCompilationUnit(CACTParser::CompilationUnitContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitTranslationUnit(CACTParser::TranslationUnitContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitFunctionFParams(CACTParser::FunctionFParamsContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitFunctionFParam(CACTParser::FunctionFParamContext *context) {
    return visitChildren(context);
}
std::any SemanticAnalyzer::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    return visitChildren(context);
}
std::any SemanticAnalyzer::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    return visitChildren(context);
}
void SemanticAnalyzer::analyze() {
}
std::any SemanticAnalyzer::visitAddOp(CACTParser::AddOpContext *context) {
    return visitChildren(context);
}
