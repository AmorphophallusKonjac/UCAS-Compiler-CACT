#include "IRGenerator.h"
std::any IRGenerator::visitCompilationUnit(CACTParser::CompilationUnitContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitFunctionType(CACTParser::FunctionTypeContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitBasicType(CACTParser::BasicTypeContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitPrimaryExpression(CACTParser::PrimaryExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitUnaryExpression(CACTParser::UnaryExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitMultiplicativeExpression(CACTParser::MultiplicativeExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitRelationalExpression(CACTParser::RelationalExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitEqualityExpression(CACTParser::EqualityExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitLogicalAndExpression(CACTParser::LogicalAndExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitExpression(CACTParser::ExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitCondition(CACTParser::ConditionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitDeclaration(CACTParser::DeclarationContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitConstantDeclaration(CACTParser::ConstantDeclarationContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitConstantDefinition(CACTParser::ConstantDefinitionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitConstantInitValue(CACTParser::ConstantInitValueContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitVariableDeclaration(CACTParser::VariableDeclarationContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitVariableDefinition(CACTParser::VariableDefinitionContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitStatement(CACTParser::StatementContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitCompoundStatement(CACTParser::CompoundStatementContext *context) {
    currentBlock = context->thisblockinfo;
    visitChildren(context);
    currentBlock = currentBlock->getParentBlock();
    return {};
}
std::any IRGenerator::visitBlockItemList(CACTParser::BlockItemListContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitBlockItem(CACTParser::BlockItemContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitExpressionStatement(CACTParser::ExpressionStatementContext *context) {
    bool hasLVal = (context->lValue() != nullptr);// 有一个赋值行为
    if (hasLVal) {
        auto *expVal = std::any_cast<IRValue *>(visit(context->expression()));
        auto *ptr = std::any_cast<IRValue *>(visit(context->lValue()));
    }
    return {};
}
std::any IRGenerator::visitLValue(CACTParser::LValueContext *context) {
    auto symbol = currentBlock->lookUpSymbol(context->Identifier()->getText());
    auto varPtr = symbol->getIRValue();
    auto size =
            dynamic_cast<IRSequentialType *>(varPtr)->getElementType()->getPrimitiveSize();
    IRValue *ret = nullptr;
    if (!context->expression().empty()) {// 左值是数组
        auto arraySize = symbol->getArraySize();
        for (int i = 0; i < context->expression().size(); ++i) {
            auto idxSize = std::accumulate(arraySize.begin() + i + 1, arraySize.end(), size, std::multiplies());
            auto idx = std::any_cast<IRValue *>(visit(context->expression(i)));
        }
    } else {
    }
    return {};
}
std::any IRGenerator::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    currentBlock = context->thisblockinfo;
    visitChildren(context);
    currentBlock = currentBlock->getParentBlock();
    return {};
}
std::any IRGenerator::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitJumpStatement(CACTParser::JumpStatementContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitTranslationUnit(CACTParser::TranslationUnitContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    currentFunc = globalBlock->lookUpFunc(context->Identifier()->getText());
    currentIRFunc = dynamic_cast<IRFunction *>(currentFunc->getIRValue());
    currentIRBasicBlock = currentIRFunc->getEntryBlock();
    return visitChildren(context);
}
std::any IRGenerator::visitFunctionFParams(CACTParser::FunctionFParamsContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitFunctionFParam(CACTParser::FunctionFParamContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    return visitChildren(context);
}
std::any IRGenerator::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    return visitChildren(context);
}
IRGenerator::IRGenerator(GlobalBlock *globalBlock, IRModule *ir, tree::ParseTree *root)
    : globalBlock(globalBlock), ir(ir), root(root), currentBlock(globalBlock), currentFunc(nullptr), currentIRFunc(nullptr), currentIRBasicBlock(nullptr) {
}
void IRGenerator::generate() {
    visit(root);
}
std::any IRGenerator::visitAddOp(CACTParser::AddOpContext *context) {
    return visitChildren(context);
}
