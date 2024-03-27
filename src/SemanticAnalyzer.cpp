#include "SemanticAnalyzer.h"
#include "CACT.h"
#include <vector>

SemanticAnalyzer::SemanticAnalyzer(std::ifstream *stream) : input(*stream), lexer(&input), tokens(&lexer),
                                                            parser(&tokens), globalBlock() {
    root = this->parser.compilationUnit();
    this->currentBlock = &globalBlock;
    if (this->parser.getNumberOfSyntaxErrors() > 0 || this->lexer.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "lexer error: " << lexer.getNumberOfSyntaxErrors() << std::endl;
        std::cerr << "parser error: " << parser.getNumberOfSyntaxErrors() << std::endl;
        throw std::runtime_error("Syntax analysis failed");
    }
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

std::any SemanticAnalyzer::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {//这里对于logicalor函数的修改还有待斟酌
    std::vector<bool> condAnd;
    for(auto logicalandexpression: context->logicalAndExpression()) {
        condAnd.push_back(logicalandexpression->cond);
    }
    context->cond = std::accumulate(condAnd.begin(), condAnd.end(), false, std::logical_or<bool>());//将所有结果或起来得到cond的bool值

    if(context->cond != false && context->cond != true){
        ErrorHandler::printErrorContext(context, "Error logicalorcondition bool value");
        throw std::runtime_error("Semantic analysis failed");
    }
    return { };
}

std::any SemanticAnalyzer::visitExpression(CACTParser::ExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    if (context->number() == nullptr) {
        if (context->dataType != DataType::BOOL) {
            ErrorHandler::printErrorContext(context,
                                            "Error value type " + Utils::ttos(context->dataType) + ", expect " +
                                            Utils::ttos(DataType::BOOL));
            throw std::runtime_error("Semantic analysis failed");
        }
    } else {
        context->number()->dataType = context->dataType;
        this->visit(context->number());
    }
    return {};
}

std::any SemanticAnalyzer::visitCondition(CACTParser::ConditionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitDeclaration(CACTParser::DeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantDeclaration(CACTParser::ConstantDeclarationContext *context) {
    context->dataType = Utils::stot(context->basicType()->getText());
    for (auto constantDefinition: context->constantDefinition()) {
        constantDefinition->dataType = context->dataType;
        auto constantInfo = std::any_cast<std::tuple<std::string, std::vector<int>, size_t, size_t>>(
                this->visit(constantDefinition));
        std::string name;
        int dimension;
        std::vector<int> arraySize;
        int line;
        std::tie(name, arraySize, dimension, line) = constantInfo;
        if (dimension == 0) {
            this->currentBlock->addNewConst(name, line, context->dataType);
        } else {
            this->currentBlock->addNewConstArray(name, line, context->dataType, arraySize, dimension);
        }
    }
    return {};
}

std::any SemanticAnalyzer::visitConstantDefinition(CACTParser::ConstantDefinitionContext *context) {
    size_t line = context->Identifier()->getSymbol()->getLine();
    for (auto size: context->IntegerConstant()) {
        context->arraySize.push_back(stoi(size->getText()));
    }
    size_t dimension = context->arraySize.size();
    std::string name = context->Identifier()->getText();
    context->constantInitValue()->dataType = context->dataType;
    context->constantInitValue()->arraySize = context->arraySize;
    this->visit(context->constantInitValue());
    return std::make_tuple(name, context->arraySize, dimension, line);
}

std::any SemanticAnalyzer::visitConstantInitValue(CACTParser::ConstantInitValueContext *context) {
    int currentSize = 0;
    int arraySize = 0;
    int subArraySize = 0;
    if (!context->arraySize.empty()) {
        arraySize = std::accumulate(context->arraySize.begin(), context->arraySize.end(), 1, std::multiplies<>());
        subArraySize = arraySize / context->arraySize.front();
    }
    if (context->constantExpression() == nullptr) {
        if (arraySize == 0) {
            ErrorHandler::printErrorContext(context, "Error init value");
            throw std::runtime_error("Semantic analysis failed");
        }
        if (context->constantInitValue().size() == 1) { // case like {{{{1}}}}
            auto constantInitValue = context->constantInitValue(0);
            constantInitValue->dataType = context->dataType;
            constantInitValue->arraySize = context->arraySize;
            this->visit(constantInitValue);
        } else {
            for (auto constantInitValue: context->constantInitValue()) {
                constantInitValue->dataType = context->dataType;
                if (constantInitValue->constantExpression() == nullptr) { // type is array
                    if (currentSize % subArraySize > 0) {
                        ErrorHandler::printErrorContext(context, "Error init value");
                        throw std::runtime_error("Semantic analysis failed");
                    }
                    constantInitValue->dataType = context->dataType;
                    for (auto i = context->arraySize.begin() + 1; i < context->arraySize.end(); ++i) {
                        constantInitValue->arraySize.push_back(*i);
                    }
                    this->visit(constantInitValue);
                    currentSize += subArraySize;
                } else { // is value
                    ++currentSize;
                    constantInitValue->dataType = context->dataType;
                    this->visit(constantInitValue);
                }
                if (currentSize > arraySize) {
                    ErrorHandler::printErrorContext(context, "Error init value");
                    throw std::runtime_error("Semantic analysis failed");
                }
            }
        }
    } else {
        context->constantExpression()->dataType = context->dataType;
        this->visit(context->constantExpression());
    }
    return {};
}

std::any SemanticAnalyzer::visitVariableDeclaration(CACTParser::VariableDeclarationContext *context) {
    context->dataType = Utils::stot(context->basicType()->getText());
    for (auto variableDefinition: context->variableDefinition()) {
        variableDefinition->dataType = context->dataType;
        auto varInfo = std::any_cast<std::tuple<std::string, std::vector<int>, size_t, size_t>>(
                this->visit(variableDefinition));
        std::string name;
        int dimension;
        std::vector<int> arraySize;
        int line;
        std::tie(name, arraySize, dimension, line) = varInfo;
        if (dimension == 0) {
            this->currentBlock->addNewVar(name, line, context->dataType);
        } else {
            this->currentBlock->addNewVarArray(name, line, context->dataType, arraySize, dimension);
        }
    }
    return {};
}

std::any SemanticAnalyzer::visitVariableDefinition(CACTParser::VariableDefinitionContext *context) {
    size_t line = context->Identifier()->getSymbol()->getLine();
    for (auto size: context->IntegerConstant()) {
        context->arraySize.push_back(stoi(size->getText()));
    }
    size_t dimension = context->arraySize.size();
    std::string name = context->Identifier()->getText();
    if (context->constantInitValue() != nullptr) {
        context->constantInitValue()->dataType = context->dataType;
        context->constantInitValue()->arraySize = context->arraySize;
        this->visit(context->constantInitValue());
    }
    return std::make_tuple(name, context->arraySize, dimension, line);
}

std::any SemanticAnalyzer::visitStatement(CACTParser::StatementContext *context) {
    if(!context->compoundStatement()->isEmpty()){
        context->compoundStatement()->thisblockinfo = currentBlock->addNewBlock();//compoudStatement又将会作为一个新块
        this->visit(context->compoundStatement());
    }else if(!context->expressionStatement()->isEmpty()){
        this->visit(context->expressionStatement());
    }else if(!context->selectionStatement()->isEmpty()){
        context->selectionStatement()->thisblockinfo = currentBlock->addNewBlock();//selectionStatement又将会作为一个新块
        this->visit(context->selectionStatement());
    }else if(!context->iterationStatement()->isEmpty()){
        context->iterationStatement()->thisblockinfo = currentBlock->addNewBlock();//iterationStatement又将会作为一个新块
        this->visit(context->iterationStatement());
    }else if(!context->jumpStatement()->isEmpty()){
        this->visit(context->jumpStatement());
    }else{
        ErrorHandler::printErrorContext(context, "statement error");
        throw std::runtime_error("Semantic analysis failed");
    }

    return { };
}

std::any SemanticAnalyzer::visitCompoundStatement(CACTParser::CompoundStatementContext *context) {
    this->visit(context->blockItemList());
    return { };
}

std::any SemanticAnalyzer::visitBlockItemList(CACTParser::BlockItemListContext *context) {
    for (auto blockitem : context->blockItem()){
        this->visit(blockitem);
    }
    return { };
}

std::any SemanticAnalyzer::visitBlockItem(CACTParser::BlockItemContext *context) {
    if(!context->declaration()->isEmpty()){
        this->visit(context->declaration());
    }else if(!context->statement()->isEmpty()){
        this->visit(context->statement());
    }else{
        ErrorHandler::printErrorContext(context, "blockitem error");
        throw std::runtime_error("Semantic analysis failed");
    }

    return { };
}

std::any SemanticAnalyzer::visitExpressionStatement(CACTParser::ExpressionStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitLValue(CACTParser::LValueContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    currentBlock = context->thisblockinfo;//更新currentblock

    this->visit(context->condition());
    context->cond = context->condition()->cond;//每一个condition最终都得给出自己的true或false
    //这里考虑在下一级的condition那里判断最终是否能返回true或false的bool类型？

    for(auto stmt : context->statement()){
        this->visit(stmt);
    }//单纯的对自己的子树statement进行访问

    return { };
}

std::any SemanticAnalyzer::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    currentBlock = context->thisblockinfo;

    this->visit(context->condition());
    context->cond = context->condition()->cond;//每一个condition最终都得给出自己的true或false
    //这里考虑在下一级的condition那里判断最终是否能返回true或false的bool类型？

    this->visit(context->statement());
    //单纯的对自己的子树statement进行访问
    return { };
}

std::any SemanticAnalyzer::visitJumpStatement(CACTParser::JumpStatementContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitCompilationUnit(CACTParser::CompilationUnitContext *context) {
    this->visit(context->translationUnit());//直接访问翻译单元即可
    return { };
}

std::any SemanticAnalyzer::visitTranslationUnit(CACTParser::TranslationUnitContext *context) {
    for (auto externalDeclaration: context->externalDeclaration()) {
        this->visit(externalDeclaration);
        //this->globalBlock.addNewBlock()关于globalblock的操作，在初始化block时就放进去//感觉没有必要维护一个block？
    }
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    if(!context->declaration()->isEmpty()){
        this->visit(context->declaration());//visit子节点
    }else if(!context->functionDefinition()->isEmpty()){
        this->visit(context->functionDefinition());//visit子节点
    }else{
        ErrorHandler::printErrorContext(context, "externaldeclaration error");
        throw std::runtime_error("Semantic analysis failed");
    }
    return {};
}

std::any SemanticAnalyzer::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    std::string returnTypeText = context->functionType()->getText();
    DataType returnType;
    returnType = Utils::stot(returnTypeText);

    context->thisfuncinfo =  globalBlock.addNewFunc(context->Identifier()->getText(), context->Identifier()->getSymbol()->getLine(),returnType);
    //全局块中的函数表添加，同时获得这个funcdefinition的funcsymbolinfo，为将来的blockinfo初始化做准备
    if (!context->functionFParams()->isEmpty()) {
        if(context->Identifier()->getText() == std::string("main")){
            ErrorHandler::printErrorContext(context, "main function must be without params");//main函数不能带有参数
            throw std::runtime_error("Semantic analysis failed");
        }else{
            context->functionFParams()->thisfuncinfo = context->thisfuncinfo;
            this->visit(context->functionFParams());//先去访问参数，在将参数都访问完之后可以获得一个完整的函数定义，再去定义blockinfo
            //等待下面的参数层完善这个函数
        }
    }
    context->thisblockinfo = globalBlock.addNewBlock(context->thisfuncinfo);//更新blockinfo

    currentBlock = context->thisblockinfo;//更新currentBlock
    currentFunc = context->thisfuncinfo;//更新currentFunc
    context->compoundStatement()->thisblockinfo = context->thisblockinfo;//这个compoundStatement作为新的block
    this->visit(context->compoundStatement());//进入函数体
    //currentFunc->setOp(new IRLabel(ctx->Ident()->getText()));

    //irGen->enterFunc(ctx->Ident()->getText());
    return {nullptr};
}

std::any SemanticAnalyzer::visitFunctionFParams(CACTParser::FunctionFParamsContext *context) {
    for (auto fparam: context->functionFParam()) {
        fparam->thisfuncinfo = context->thisfuncinfo;//继续将函数往下传
        this->visit(fparam);//具体参数
    }
    return {nullptr};
}

std::any SemanticAnalyzer::visitFunctionFParam(CACTParser::FunctionFParamContext *context) {
    std::string basicTypeText = context->basicType()->getText();
    DataType basicType;
    basicType = Utils::stot(basicTypeText);

    int dimension;
    dimension = context->LeftBracket().size();//计算维数
    if(!dimension){
        context->thisfuncinfo->addParamVar(context->Identifier()->getText(), context->Identifier()->getSymbol()->getLine(), basicType);
    }else{
        int valid_size;//标记了数字的个数//第一维可能标记为0
        valid_size = context->IntegerConstant().size();
        std::vector<int> param_array;
        
        if(valid_size == dimension){
            for(auto integetconstant : context->IntegerConstant()){
                param_array.push_back(stoi(integetconstant->getText()));
            }
        }else if(valid_size == (dimension-1)){
            param_array.push_back(0);
            for(auto integetconstant : context->IntegerConstant()){
                param_array.push_back(stoi(integetconstant->getText()));
            }
        }else{
            ErrorHandler::printErrorContext(context, "array dimension error");//main函数不能带有参数
            throw std::runtime_error("Semantic analysis failed");
        }//分析得到paramlist

        context->thisfuncinfo->addParamArray(context->Identifier()->getText(), context->Identifier()->getSymbol()->getLine(), basicType, param_array, dimension);
    }

    return {nullptr};
}

void SemanticAnalyzer::analyze() {
    this->visit(this->root);
}

std::any SemanticAnalyzer::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    if (context->dataType != DataType::INT) {
        ErrorHandler::printErrorContext(context,
                                        "Error value type " + Utils::ttos(context->dataType) + ", expect " +
                                        Utils::ttos(DataType::INT));
        throw std::runtime_error("Semantic analysis failed");
    }
    return {};
}

std::any SemanticAnalyzer::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    unsigned length = context->FloatingConstant()->getText().size();
    char suffix = context->FloatingConstant()->getText()[length - 1];
    if (suffix == 'f' || suffix == 'F') {
        if (context->dataType != DataType::FLOAT) {
            ErrorHandler::printErrorContext(context,
                                            "Error value type " + Utils::ttos(context->dataType) + ", expect " +
                                            Utils::ttos(DataType::FLOAT));
            throw std::runtime_error("Semantic analysis failed");
        }
    } else {
        if (context->dataType != DataType::DOUBLE) {
            ErrorHandler::printErrorContext(context,
                                            "Error value type " + Utils::ttos(context->dataType) + ", expect " +
                                            Utils::ttos(DataType::DOUBLE));
            throw std::runtime_error("Semantic analysis failed");
        }
    }
    return {};
}
