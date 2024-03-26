#include "SemanticAnalyzer.h"

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

std::any SemanticAnalyzer::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitExpression(CACTParser::ExpressionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    if (context->number()->isEmpty()) {
        if (context->dataType != DataType::BOOL) {
            errorHandler(context, "Error value type");
            throw std::runtime_error("Semantic analysis error");
        }
    }
    else {
        context->number()->dataType = context->dataType;
        this->visit(context->number());
    }
}

std::any SemanticAnalyzer::visitCondition(CACTParser::ConditionContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitDeclaration(CACTParser::DeclarationContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitConstantDeclaration(CACTParser::ConstantDeclarationContext *context) {
    std::string basicType(context->basicType()->getText());
    if (basicType == "int") {
        context->dataType = DataType::INT;
    } else if (basicType == "double") {
        context->dataType = DataType::DOUBLE;
    } else if (basicType == "float") {
        context->dataType = DataType::FLOAT;
    } else {
        context->dataType = DataType::BOOL;
    }
    for (auto constantDefinition : context->constantDefinition()) {
        constantDefinition->dataType = context->dataType;
        auto constantInfo = std::any_cast<std::tuple<std::string, std::vector<int>, int, int>>(this->visit(constantDefinition));
        std::string name;
        int dimension;
        std::vector <int> arraySize;
        int line;
        std::tie(name, arraySize, dimension, line) = constantInfo;
        if (dimension == 0) {
            this->currentBlock->addNewConst(name, line, context->dataType);
        }
        else {
            this->currentBlock->addNewConstArray(name, line, context->dataType, arraySize, dimension);
        }
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitConstantDefinition(CACTParser::ConstantDefinitionContext *context) {
    int line = context->Identifier()->getSymbol()->getLine();
    for (auto size : context->IntegerConstant()) {
        context->arraySize.push_back(stoi(size->getText()));
    }
    int dimmension = context->arraySize.size();
    std::string name = context->Identifier()->getText();
    context->constantInitValue()->dataType = context->dataType;
    context->constantInitValue()->arraySize = context->arraySize;
    this->visit(context->constantInitValue());
    return std::make_tuple(name, context->arraySize, dimmension, line);
}

std::any SemanticAnalyzer::visitConstantInitValue(CACTParser::ConstantInitValueContext *context) {
    int currentSize = 0;
    int arraySize = 0;
    int subArraySize = 0;
    if (!context->arraySize.empty()) {
        arraySize = std::accumulate(context->arraySize.begin(), context->arraySize.end(), 1, std::multiplies<int>());
        subArraySize = arraySize / context->arraySize.front();
    }
    if (context->constantExpression()->isEmpty()) {
        if (arraySize == 0) {
            errorHandler(context, "Error init value");
            throw std::runtime_error("Semantic analysis failed");
        }
        for (auto constantInitValue : context->constantInitValue()) {
            constantInitValue->dataType = context->dataType;
            if (constantInitValue->constantExpression()->isEmpty()) { // is array
                if (currentSize % subArraySize > 0) {
                    errorHandler(context, "Error init value");
                    throw std::runtime_error("Semantic analysis failed");
                }
                constantInitValue->dataType = context->dataType;
                for (auto i = context->arraySize.begin() + 1; i < context->arraySize.end(); ++i) {
                    constantInitValue->arraySize.push_back(*i);
                }
                this->visit(constantInitValue);
                currentSize += subArraySize;
            }
            else { // is value
                ++currentSize;
                constantInitValue->constantExpression()->dataType = context->dataType;
                this->visit(constantInitValue->constantExpression());
            }
            if (currentSize > arraySize) {
                errorHandler(context, "Error init value");
                throw std::runtime_error("Semantic analysis failed");
            }
        }
    }
    else {
            context->constantExpression()->dataType = context->dataType;
            this->visit(context->constantExpression());
    }
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

void SemanticAnalyzer::analyze() {
    this->visit(this->root);
}

std::any SemanticAnalyzer::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    if (context->dataType != DataType::INT) {
        errorHandler(context, "Error value type");
        throw std::runtime_error("Semantic analysis error");
    }
}

std::any SemanticAnalyzer::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    unsigned length = context->FloatingConstant()->getText().size();
    char suffix = context->FloatingConstant()->getText()[length - 1];
    if (suffix == 'f' || suffix == 'F') {
        if (context->dataType != DataType::FLOAT) {
            errorHandler(context, "Error value type");
            throw std::runtime_error("Semantic analysis error");
        }
    }
    else {
        if (context->dataType != DataType::DOUBLE) {
            errorHandler(context, "Error value type");
            throw std::runtime_error("Semantic analysis error");
        }
    }
}

void SemanticAnalyzer::errorHandler(ParserRuleContext *context, std::string msg) {
    size_t line = context->start->getLine();
    size_t column = context->start->getCharPositionInLine();
    std::cerr << "Line " << std::to_string(line) << ":" << std::to_string(column) << " '" << context->getText() << "' " << msg << std::endl;
}

