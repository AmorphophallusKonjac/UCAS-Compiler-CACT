#include "IRGenerator.h"

#include "IR/IRConstant.h"
#include "IR/iMemory.h"
#include "IR/iOperators.h"
#include "IR/iOther.h"

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
    if (context->lValue()) {
        auto ret = std::any_cast<IRValue *>(visit(context->lValue()));
        if (context->lValue()->loadable) {
            ret = dynamic_cast<IRValue *>(new IRLoadInst(
                    ret, std::to_string(currentIRFunc->getCount()), currentIRBasicBlock));
        }
        return ret;
    } else if (context->number()) {
        return visit(context->number());
    } else {
        return visit(context->expression());
    }
    return visitChildren(context);
}

std::any IRGenerator::visitUnaryExpression(CACTParser::UnaryExpressionContext *context) {
    if (context->primaryExpression()) {
        return visit(context->primaryExpression());
    } else if (context->unaryOperator()) {
        auto val = std::any_cast<IRValue *>(visit(context->unaryExpression()));
        std::string opSt = context->unaryOperator()->getText();
        if (opSt == "+") {
            return val;
        } else if (opSt == "-") {
            auto ret = dynamic_cast<IRValue *>(IRBinaryOperator::createNeg(
                    val, std::to_string(currentIRFunc->getCount()), currentIRBasicBlock));
            currentIRFunc->addCount();
            return ret;
        } else {  // "!"
            auto ret = dynamic_cast<IRValue *>(IRBinaryOperator::createNot(
                    val, std::to_string(currentIRFunc->getCount()), currentIRBasicBlock));
            currentIRFunc->addCount();
            return ret;
        }
    } else {  // function

        std::vector<IRValue *> rParams;
        if (context->functionRParams()) {
            rParams = std::any_cast<std::vector<IRValue *>>(visit(context->functionRParams()));
        }
        auto func = globalBlock->lookUpFunc(context->Identifier()->getText())->getIRValue();
        auto ret = dynamic_cast<IRValue *>(new IRCallInst(
                func, rParams, std::to_string(currentIRFunc->getCount()), currentIRBasicBlock));
        currentIRFunc->addCount();
        return ret;
    }
}

std::any IRGenerator::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    std::vector<IRValue *> rParams;
    for (auto param: context->expression()) {
        rParams.push_back(std::any_cast<IRValue *>(visit(param)));
    }
    return rParams;
}

std::any IRGenerator::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitMultiplicativeExpression(
        CACTParser::MultiplicativeExpressionContext *context) {
    auto ret = std::any_cast<IRValue *>(visit(context->unaryExpression(0)));
    auto len = context->unaryExpression().size();
    for (int i = 1; i < len; ++i) {
        auto val = std::any_cast<IRValue *>(visit(context->unaryExpression(i)));
        IRInstruction::BinaryOps op;
        std::string opSt = context->multiplicativeOp(i - 1)->getText();
        if (opSt == "*") {
            op = IRInstruction::Mul;
        } else if (opSt == "/") {
            op = IRInstruction::Div;
        } else {  // "%"
            op = IRInstruction::Rem;
        }
        ret = IRBinaryOperator::create(op, ret, val, std::to_string(currentIRFunc->getCount()),
                                       currentIRBasicBlock);
        currentIRFunc->addCount();
    }
    return ret;
}

std::any IRGenerator::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    auto ret = std::any_cast<IRValue *>(visit(context->multiplicativeExpression(0)));
    auto len = context->multiplicativeExpression().size();
    for (int i = 1; i < len; ++i) {
        auto val = std::any_cast<IRValue *>(visit(context->multiplicativeExpression(i)));
        IRInstruction::BinaryOps op;
        std::string opSt = context->additiveOp(i - 1)->getText();
        if (opSt == "+") {  // "+"
            op = IRInstruction::Add;
        } else {  // "-"
            op = IRInstruction::Sub;
        }
        ret = IRBinaryOperator::create(op, ret, val, std::to_string(currentIRFunc->getCount()),
                                       currentIRBasicBlock);
        currentIRFunc->addCount();
    }
    return ret;
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
    IRValue *ret = nullptr;
    if (context->additiveExpression()) {  // 加法
        ret = std::any_cast<IRValue *>(visit(context->additiveExpression()));
    } else {  // 布尔常量
        if (context->BooleanConstant()->getText() == "true") {
            ret = dynamic_cast<IRValue *>(IRConstantBool::get(true));
        } else {
            ret = dynamic_cast<IRValue *>(IRConstantBool::get(false));
        }
    }
    return ret;
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
    bool hasLVal = (context->lValue() != nullptr);  // 有一个赋值行为
    if (hasLVal) {
        auto *expVal = std::any_cast<IRValue *>(visit(context->expression()));
        auto *ptr = std::any_cast<IRValue *>(visit(context->lValue()));
        new IRStoreInst(expVal, ptr, currentIRBasicBlock);
    }
    return {};
}

std::any IRGenerator::visitLValue(CACTParser::LValueContext *context) {
    auto symbol = currentBlock->lookUpSymbol(context->Identifier()->getText());
    auto varPtr = symbol->getIRValue();
    if (symbol->getSymbolType() != SymbolType::CONST) {
        auto size =
                dynamic_cast<IRSequentialType *>(varPtr->getType())->getElementType()->getPrimitiveSize();
        if (!context->expression().empty()) {  // 左值是数组
            auto arraySize = symbol->getArraySize();
            for (int i = 0; i < context->expression().size(); ++i) {
                auto idxSize = dynamic_cast<IRValue *>(IRConstantInt::get(std::accumulate(
                        arraySize.begin() + i + 1, arraySize.end(), size, std::multiplies())));
                auto idx = std::any_cast<IRValue *>(visit(context->expression(i)));
                auto offset = dynamic_cast<IRValue *>(IRBinaryOperator::create(
                        IRInstruction::Mul, idx, idxSize, std::to_string(currentIRFunc->getCount()),
                        currentIRBasicBlock));
                currentIRFunc->addCount();
                varPtr = dynamic_cast<IRValue *>(IRBinaryOperator::create(
                        IRInstruction::Add, varPtr, offset, std::to_string(currentIRFunc->getCount()),
                        currentIRBasicBlock));
                currentIRFunc->addCount();
            }
        }
    }
    if (context->expression().size() < symbol->getArraySize().size() ||
        symbol->getSymbolType() == SymbolType::CONST) {
        context->loadable = false;
    } else {
        context->loadable = true;
    }
    return varPtr;
}

std::any IRGenerator::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    currentBlock = context->thisblockinfo;

    IRBasicBlock *trueBlock = nullptr;
    IRBasicBlock *falseBlock = nullptr;
    IRBasicBlock *nextBlock = nullptr;
    if (context->Else()) {
        trueBlock = new IRBasicBlock();
        falseBlock = new IRBasicBlock();
        nextBlock = new IRBasicBlock();
    } else {
        trueBlock = new IRBasicBlock();
        falseBlock = new IRBasicBlock();
        nextBlock = falseBlock;
    }
    context->condition()->trueBlock = trueBlock;
    context->condition()->falseBlock = falseBlock;
    visit(context->condition());

    //! visit true statement
    trueBlock->setParent(currentIRFunc);
    currentIRFunc->addBasicBlock(trueBlock);
    trueBlock->setName(std::to_string(currentIRFunc->getCount()));
    currentIRFunc->addCount();
    currentIRBasicBlock = trueBlock;
    visit(context->statement(0));

    //! visit false statement
    if (context->Else()) {
        falseBlock->setParent(currentIRFunc);
        currentIRFunc->addBasicBlock(falseBlock);
        falseBlock->setName(std::to_string(currentIRFunc->getCount()));
        currentIRFunc->addCount();
        currentIRBasicBlock = falseBlock;
        visit(context->statement(1));
    }


    currentBlock = currentBlock->getParentBlock();
    return {};
}

std::any IRGenerator::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    currentBlock = context->thisblockinfo;
    visitChildren(context);
    currentBlock = currentBlock->getParentBlock();
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
    return dynamic_cast<IRValue *>(IRConstantInt::get(std::stoi(context->getText())));
}

std::any IRGenerator::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    std::string st = context->getText();
    if (st[st.size() - 1] == 'f' || st[st.size() - 1] == 'F') {  // float
        return dynamic_cast<IRValue *>(IRConstantFloat::get(std::stof(st)));
    } else {  // double
        return dynamic_cast<IRValue *>(IRConstantDouble::get(std::stod(st)));
    }
}

IRGenerator::IRGenerator(GlobalBlock *globalBlock, IRModule *ir, tree::ParseTree *root)
        : globalBlock(globalBlock),
          ir(ir),
          root(root),
          currentBlock(globalBlock),
          currentFunc(nullptr),
          currentIRFunc(nullptr),
          currentIRBasicBlock(nullptr) {}

void IRGenerator::generate() { visit(root); }

std::any IRGenerator::visitAddOp(CACTParser::AddOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitMultiplicativeOp(CACTParser::MultiplicativeOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitAdditiveOp(CACTParser::AdditiveOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitRelationalOp(CACTParser::RelationalOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitEqualityOp(CACTParser::EqualityOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitLogicalAndOp(CACTParser::LogicalAndOpContext *context) {
    return visitChildren(context);
}

std::any IRGenerator::visitLogicalOrOp(CACTParser::LogicalOrOpContext *context) {
    return visitChildren(context);
}
