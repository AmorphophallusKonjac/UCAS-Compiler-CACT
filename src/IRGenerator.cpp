#include "IRGenerator.h"
std::any IRGenerator::visitCompilationUnit(CACTParser::CompilationUnitContext *context) {
    return std::any();
}
std::any IRGenerator::visitFunctionType(CACTParser::FunctionTypeContext *context) {
    return std::any();
}
std::any IRGenerator::visitBasicType(CACTParser::BasicTypeContext *context) {
    return std::any();
}
std::any IRGenerator::visitPrimaryExpression(CACTParser::PrimaryExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitUnaryExpression(CACTParser::UnaryExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    return std::any();
}
std::any IRGenerator::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return std::any();
}
std::any IRGenerator::visitMultiplicativeExpression(CACTParser::MultiplicativeExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitRelationalExpression(CACTParser::RelationalExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitEqualityExpression(CACTParser::EqualityExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitLogicalAndExpression(CACTParser::LogicalAndExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitExpression(CACTParser::ExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    return std::any();
}
std::any IRGenerator::visitCondition(CACTParser::ConditionContext *context) {
    return std::any();
}
std::any IRGenerator::visitDeclaration(CACTParser::DeclarationContext *context) {
    return std::any();
}
std::any IRGenerator::visitConstantDeclaration(CACTParser::ConstantDeclarationContext *context) {
    return std::any();
}
std::any IRGenerator::visitConstantDefinition(CACTParser::ConstantDefinitionContext *context) {
    return std::any();
}
std::any IRGenerator::visitConstantInitValue(CACTParser::ConstantInitValueContext *context) {
    return std::any();
}
std::any IRGenerator::visitVariableDeclaration(CACTParser::VariableDeclarationContext *context) {
    return std::any();
}
std::any IRGenerator::visitVariableDefinition(CACTParser::VariableDefinitionContext *context) {
    return std::any();
}
std::any IRGenerator::visitStatement(CACTParser::StatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitCompoundStatement(CACTParser::CompoundStatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitBlockItemList(CACTParser::BlockItemListContext *context) {
    return std::any();
}
std::any IRGenerator::visitBlockItem(CACTParser::BlockItemContext *context) {
    return std::any();
}
std::any IRGenerator::visitExpressionStatement(CACTParser::ExpressionStatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitLValue(CACTParser::LValueContext *context) {
    return std::any();
}
std::any IRGenerator::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitJumpStatement(CACTParser::JumpStatementContext *context) {
    return std::any();
}
std::any IRGenerator::visitTranslationUnit(CACTParser::TranslationUnitContext *context) {
    return std::any();
}
std::any IRGenerator::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    return std::any();
}
std::any IRGenerator::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    return std::any();
}
std::any IRGenerator::visitFunctionFParams(CACTParser::FunctionFParamsContext *context) {
    return std::any();
}
std::any IRGenerator::visitFunctionFParam(CACTParser::FunctionFParamContext *context) {
    return std::any();
}
std::any IRGenerator::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    return std::any();
}
std::any IRGenerator::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    return std::any();
}
IRGenerator::IRGenerator(GlobalBlock *globalBlock, IRModule *ir, tree::ParseTree *root)
    : globalBlock(globalBlock), ir(ir), root(root), currentBlock(globalBlock), currentFunc(nullptr) {
}
void IRGenerator::generate() {
    visit(root);
}
