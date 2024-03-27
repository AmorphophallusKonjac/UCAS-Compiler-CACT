#include "SemanticAnalyzer.h"
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
    if (context->primaryExpression() != nullptr) {

    } else if (context->unaryOperator() != nullptr) {

    } else { // function

    }
    return {};
}

std::any SemanticAnalyzer::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return {};
}

std::any SemanticAnalyzer::visitMultiplicativeExpression(CACTParser::MultiplicativeExpressionContext *context) {
    for (auto multiplicativeExpression: context->unaryExpression()) {
        this->visit(multiplicativeExpression);
    }
    context->dataType = context->unaryExpression( 0)->dataType;
    for (auto multiplicativeExpression: context->unaryExpression()) {
        if (context->dataType != multiplicativeExpression->dataType) {
            ErrorHandler::printErrorContext(multiplicativeExpression, "Error data type, expect " + Utils::ttos(context->dataType));
            throw std::runtime_error("Semantic analysis failed");
        }
    }
    return {};
}

std::any SemanticAnalyzer::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    for (auto multiplicativeExpression: context->multiplicativeExpression()) {
        this->visit(multiplicativeExpression);
    }
    context->dataType = context->multiplicativeExpression(0)->dataType;
    for (auto multiplicativeExpression: context->multiplicativeExpression()) {
        if (context->dataType != multiplicativeExpression->dataType) {
            ErrorHandler::printErrorContext(multiplicativeExpression, "Error data type, expect " + Utils::ttos(context->dataType));
            throw std::runtime_error("Semantic analysis failed");
        }
    }
    return {};
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
    if (context->additiveExpression() == nullptr) {
        context->dataType = DataType::BOOL;
    } else {
        this->visit(context->additiveExpression());
        context->dataType = context->additiveExpression()->dataType;
    }
    return {};
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
    for (auto externalDeclaration: context->externalDeclaration()) {
        this->visit(externalDeclaration);
        this->currentBlock = externalDeclaration->thisblockinfo;
        //this->globalBlock.addNewBlock()关于globalblock的操作，在初始化block时就放进去//感觉没有必要维护一个block？
    }
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    if (context->declaration()->isEmpty()) {
        this->visit(context->functionDefinition());//先visit子节点
        context->thisblockinfo = context->functionDefinition()->thisblockinfo;
        this->currentBlock = context->functionDefinition()->thisblockinfo;//更新currentblock以及自己的blockinfo属性
    } else {
        this->visit(context->declaration());//先visit子节点
        context->thisblockinfo = context->declaration()->thisblockinfo;
        this->currentBlock = context->declaration()->thisblockinfo;//更新currentblock以及自己的blockinfo属性
    }
    return {};
}

std::any SemanticAnalyzer::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    std::string returnTypeText = context->functionType()->getText();
    DataType returnType;
    if (returnTypeText == "bool") {
        returnType = DataType::BOOL;
    } else if (returnTypeText == "int") {
        returnType = DataType::INT;
    } else if (returnTypeText == "float") {
        returnType = DataType::FLOAT;
    } else if (returnTypeText == "double") {
        returnType = DataType::DOUBLE;
    } else if (returnTypeText == "void") {
        returnType = DataType::VOID;
    } else {
        throw std::runtime_error(std::string("unknown function return value type: ") + returnTypeText);
        return nullptr;
    }

    context->thisfuncinfo = globalBlock.addNewFunc(context->Identifier()->getText(),
                                                   context->Identifier()->getSymbol()->getLine(), returnType);
    //全局块中的函数表添加，同时获得这个funcdefinition的funcsymbolinfo，为将来的blockinfo初始化做准备
    if (!context->functionFParams()->isEmpty()) {
        if (context->Identifier()->getText() == std::string("main")) {
            ErrorHandler::printErrorContext(context, "main function must be without params");//main函数不能带有参数
            throw std::runtime_error("Semantic analysis failed");
        } else {
            context->functionFParams()->thisfuncinfo = context->thisfuncinfo;
            this->visit(context->functionFParams());//先去访问参数，在将参数都访问完之后可以获得一个完整的函数定义，再去定义blockinfo
            //等待下面的参数层完善这个函数
        }
    }
    context->thisblockinfo = globalBlock.addNewBlock(context->thisfuncinfo);
    currentFunc = context->thisfuncinfo;//更新currentFunc
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
    if (basicTypeText == "bool") {
        basicType = DataType::BOOL;
    } else if (basicTypeText == "int") {
        basicType = DataType::INT;
    } else if (basicTypeText == "float") {
        basicType = DataType::FLOAT;
    } else if (basicTypeText == "double") {
        basicType = DataType::DOUBLE;
    } else if (basicTypeText == "void") {
        basicType = DataType::VOID;
    } else {
        throw std::runtime_error(std::string("unknown function return value type: ") + basicTypeText);
        return nullptr;
    }

    int dimension;
    dimension = context->LeftBracket().size();//计算维数
    if (!dimension) {
        context->thisfuncinfo->addParamVar(context->Identifier()->getText(),
                                           context->Identifier()->getSymbol()->getLine(), basicType);
    } else {
        int valid_size;//标记了数字的个数//第一维可能标记为0
        valid_size = context->IntegerConstant().size();
        std::vector<int> param_array;

        if (valid_size == dimension) {
            for (auto integetconstant: context->IntegerConstant()) {
                param_array.push_back(stoi(integetconstant->getText()));
            }
        } else if (valid_size == (dimension - 1)) {
            param_array.push_back(0);
            for (auto integetconstant: context->IntegerConstant()) {
                param_array.push_back(stoi(integetconstant->getText()));
            }
        } else {
            ErrorHandler::printErrorContext(context, "array dimension error");//main函数不能带有参数
            throw std::runtime_error("Semantic analysis failed");
        }//分析得到paramlist

        context->thisfuncinfo->addParamArray(context->Identifier()->getText(),
                                             context->Identifier()->getSymbol()->getLine(), basicType, param_array,
                                             dimension);
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
