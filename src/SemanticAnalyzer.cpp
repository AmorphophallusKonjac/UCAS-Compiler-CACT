#include "SemanticAnalyzer.h"
#include "CACT.h"
#include <vector>
#include "ReturnValue.h"
#include "symbolTable.h"

SemanticAnalyzer::SemanticAnalyzer(std::ifstream *stream) : input(*stream), lexer(&input), tokens(&lexer),
                                                            parser(&tokens), globalBlock() {
    root = this->parser.compilationUnit();
    if (this->parser.getNumberOfSyntaxErrors() > 0 || this->lexer.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "lexer error: " << lexer.getNumberOfSyntaxErrors() << std::endl;
        std::cerr << "parser error: " << parser.getNumberOfSyntaxErrors() << std::endl;
        throw std::runtime_error("Syntax analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    this->currentBlock = &globalBlock;
    this->currentFunc = nullptr;
    // add IO function
    this->globalBlock.addNewFunc("print_int", 0, DataType::VOID)->addParamVar("x", 0, DataType::INT);
    this->globalBlock.addNewFunc("print_float", 0, DataType::VOID)->addParamVar("x", 0, DataType::FLOAT);
    this->globalBlock.addNewFunc("print_double", 0, DataType::VOID)->addParamVar("x", 0, DataType::DOUBLE);
    this->globalBlock.addNewFunc("print_bool", 0, DataType::VOID)->addParamVar("x", 0, DataType::BOOL);
    this->globalBlock.addNewFunc("get_int", 0, DataType::INT);
    this->globalBlock.addNewFunc("get_float", 0, DataType::FLOAT);
    this->globalBlock.addNewFunc("get_double", 0, DataType::DOUBLE);
}

SemanticAnalyzer::~SemanticAnalyzer() = default;

std::any SemanticAnalyzer::visitFunctionType(CACTParser::FunctionTypeContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitBasicType(CACTParser::BasicTypeContext *context) {
    return visitChildren(context);
}

std::any SemanticAnalyzer::visitPrimaryExpression(CACTParser::PrimaryExpressionContext *context) {
    if (context->lValue() != nullptr) {
        return this->visit(context->lValue());
    } else if (context->number() != nullptr) {
        return this->visit(context->number());
    } else {
        return this->visit(context->expression());
    }
}

std::any SemanticAnalyzer::visitUnaryExpression(CACTParser::UnaryExpressionContext *context) {
    DataType dataType;
    int dimension = 0;
    std::vector<int> arraySize;
    if (context->primaryExpression() != nullptr) { // primaryExpression
        return visitChildren(context);
    } else if (context->unaryOperator() != nullptr) { // unaryOperator unaryExpression
        auto unaryExpression = std::any_cast<ReturnValue>(this->visit(context->unaryExpression()));
        dataType = unaryExpression.getDataType();
        if (unaryExpression.getDimension() != 0) {
            ErrorHandler::printErrorContext(context->unaryExpression(),
                                            "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (context->unaryOperator()->getText() == "!") {
            if (dataType != DataType::BOOL) {
                ErrorHandler::printErrorContext(context->unaryExpression(),
                                                "Error data type, expect " + Utils::ttos(DataType::BOOL));
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        } else {
            if (dataType != DataType::DOUBLE &&
                dataType != DataType::FLOAT &&
                dataType != DataType::INT) {
                ErrorHandler::printErrorContext(context->unaryExpression(),
                                                "Error data type, expect " + Utils::ttos(DataType::DOUBLE) +
                                                "/" + Utils::ttos(DataType::FLOAT) +
                                                "/" + Utils::ttos(DataType::INT));
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        return unaryExpression;
    } else { // function
        auto func = globalBlock.lookUpFunc(context->Identifier()->getText());
        if (func == nullptr) {
            ErrorHandler::printErrorContext(context, "use of undeclared function");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (context->functionRParams() != nullptr) {
            context->functionRParams()->func = func;
            this->visit(context->functionRParams());
        } else {
            if (func->getparamNum() != 0) {
                ErrorHandler::printErrorContext(context, "Too few arguments");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        return ReturnValue(func->getDataType(),
                           0, std::vector<int>(),
                           func->getSymbolType());
    }
}

std::any SemanticAnalyzer::visitFunctionRParams(CACTParser::FunctionRParamsContext *context) {
    auto paramList = context->func->getparamList();

    if (context->func->getparamNum() < context->expression().size()) {
        ErrorHandler::printErrorContext(context, "Too many arguments");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    if (context->func->getparamNum() > context->expression().size()) {
        ErrorHandler::printErrorContext(context, "Too few arguments");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }

    for (int i = 0; i < paramList.size(); ++i) {
        auto retVal = std::any_cast<ReturnValue>(this->visit(context->expression()[i]));
        if (retVal.getDataType() != paramList[i]->getDataType()) {
            ErrorHandler::printErrorContext(context->expression(i), "Error data type");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        auto arraySize = paramList[i]->getArraySize();
        if (retVal.getDimension() != arraySize.size()) {
            ErrorHandler::printErrorContext(context->expression(i), "Error array dimension");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        for (int j = 0; j < retVal.getDimension(); ++j) {
            if (arraySize[j] == 0)
                continue;
            if (arraySize[j] != retVal.getArraySize()[j]) {
                ErrorHandler::printErrorContext(context->expression(i), "Error array dimension");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
    }
    return {};
}

std::any SemanticAnalyzer::visitUnaryOperator(CACTParser::UnaryOperatorContext *context) {
    return {};
}

std::any SemanticAnalyzer::visitMultiplicativeExpression(CACTParser::MultiplicativeExpressionContext *context) {
    std::vector<ReturnValue> expList;
    for (auto unaryExpression: context->unaryExpression()) {
        expList.push_back(std::any_cast<ReturnValue>(this->visit(unaryExpression)));
    }
    if (expList.size() > 1) {
        for (int i = 0; i < expList.size(); ++i) {
            if (expList[i].getDataType() != DataType::INT &&
                expList[i].getDataType() != DataType::DOUBLE &&
                expList[i].getDataType() != DataType::FLOAT) {
                ErrorHandler::printErrorContext(context->unaryExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            if (expList[i].getDimension() != 0) {
                ErrorHandler::printErrorContext(context->unaryExpression(i),
                                                "is array");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        for (int i = 1; i < expList.size(); ++i) {
            if (expList[i - 1].getDataType() != expList[i].getDataType()) {
                ErrorHandler::printErrorContext(context->unaryExpression(i - 1),
                                                "is " + Utils::ttos(expList[i - 1].getDataType()));
                ErrorHandler::printErrorContext(context->unaryExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()));
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
    }
    return expList[0];
}

std::any SemanticAnalyzer::visitAdditiveExpression(CACTParser::AdditiveExpressionContext *context) {
    std::vector<ReturnValue> expList;
    for (auto multiplicativeExpression: context->multiplicativeExpression()) {
        expList.push_back(std::any_cast<ReturnValue>(this->visit(multiplicativeExpression)));
    }

    if (expList.size() > 1) {
        for (int i = 0; i < expList.size(); ++i) {
            if (expList[i].getDataType() != DataType::INT &&
                expList[i].getDataType() != DataType::DOUBLE &&
                expList[i].getDataType() != DataType::FLOAT) {
                ErrorHandler::printErrorContext(context->multiplicativeExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            if (expList[i].getDimension() != 0) {
                ErrorHandler::printErrorContext(context->multiplicativeExpression(i),
                                                "is array");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        for (int i = 1; i < expList.size(); ++i) {
            if (expList[i - 1].getDataType() != expList[i].getDataType()) {
                ErrorHandler::printErrorContext(context->multiplicativeExpression(i - 1),
                                                "is " + Utils::ttos(expList[i - 1].getDataType()));
                ErrorHandler::printErrorContext(context->multiplicativeExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()));
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
    }
    return expList[0];
}

std::any SemanticAnalyzer::visitRelationalExpression(CACTParser::RelationalExpressionContext *context) {
    if (context->additiveExpression().size() == 1) {
        auto additiveExpression = std::any_cast<ReturnValue>(this->visit(context->additiveExpression(0)));
        if (additiveExpression.getDimension() != 0) {
            ErrorHandler::printErrorContext(context->additiveExpression(0),
                                            "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return additiveExpression;
    } else {
        std::vector<ReturnValue> expList;
        for (auto additiveExpression: context->additiveExpression()) {
            expList.push_back(std::any_cast<ReturnValue>(this->visit(additiveExpression)));
        }
        for (int i = 0; i < expList.size(); ++i) {
            if (expList[i].getDataType() != DataType::INT &&
                expList[i].getDataType() != DataType::DOUBLE &&
                expList[i].getDataType() != DataType::FLOAT) {
                ErrorHandler::printErrorContext(context->additiveExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            if (expList[i].getDimension() != 0) {
                ErrorHandler::printErrorContext(context->additiveExpression(i),
                                                "is array");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        if (expList[0].getDataType() != expList[1].getDataType()) {
            ErrorHandler::printErrorContext(context->additiveExpression(0),
                                            "is " + Utils::ttos(expList[0].getDataType()));
            ErrorHandler::printErrorContext(context->additiveExpression(1),
                                            "is " + Utils::ttos(expList[1].getDataType()));
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return ReturnValue(DataType::BOOL, 0, std::vector<int>(), SymbolType::NUM);
    }
}

std::any SemanticAnalyzer::visitEqualityExpression(CACTParser::EqualityExpressionContext *context) {
    if (context->relationalExpression().size() == 1) {
        auto relationalExpression = std::any_cast<ReturnValue>(this->visit(context->relationalExpression(0)));
        if (relationalExpression.getDimension() != 0) {
            ErrorHandler::printErrorContext(context->relationalExpression(0),
                                            "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return relationalExpression;
    } else {
        std::vector<ReturnValue> expList;
        for (auto relationalExpression: context->relationalExpression()) {
            expList.push_back(std::any_cast<ReturnValue>(this->visit(relationalExpression)));
        }
        for (int i = 0; i < expList.size(); ++i) {
            if (expList[i].getDataType() != DataType::INT &&
                expList[i].getDataType() != DataType::DOUBLE &&
                expList[i].getDataType() != DataType::FLOAT &&
                expList[i].getDataType() != DataType::BOOL) {
                ErrorHandler::printErrorContext(context->relationalExpression(i),
                                                "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            if (expList[i].getDimension() != 0) {
                ErrorHandler::printErrorContext(context->relationalExpression(i),
                                                "is array");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        if (expList[0].getDataType() != expList[1].getDataType()) {
            ErrorHandler::printErrorContext(context->relationalExpression(0),
                                            "is " + Utils::ttos(expList[0].getDataType()));
            ErrorHandler::printErrorContext(context->relationalExpression(1),
                                            "is " + Utils::ttos(expList[1].getDataType()));
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return ReturnValue(DataType::BOOL, 0, std::vector<int>(), SymbolType::NUM);
    }
}

std::any SemanticAnalyzer::visitLogicalAndExpression(CACTParser::LogicalAndExpressionContext *context) {
    std::vector<ReturnValue> expList;
    for (auto equalityExpression: context->equalityExpression()) {
        expList.push_back(std::any_cast<ReturnValue>(this->visit(equalityExpression)));
    }
    for (int i = 0; i < expList.size(); ++i) {
        if (expList[i].getDataType() != DataType::BOOL) {
            ErrorHandler::printErrorContext(context->equalityExpression(i),
                                            "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (expList[i].getDimension() != 0) {
            ErrorHandler::printErrorContext(context->equalityExpression(i),
                                            "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
    }
    return expList[0];
}

std::any SemanticAnalyzer::visitLogicalOrExpression(CACTParser::LogicalOrExpressionContext *context) {
    std::vector<ReturnValue> expList;
    for (auto equalityExpression: context->logicalAndExpression()) {
        expList.push_back(std::any_cast<ReturnValue>(this->visit(equalityExpression)));
    }
    for (int i = 0; i < expList.size(); ++i) {
        if (expList[i].getDataType() != DataType::BOOL) {
            ErrorHandler::printErrorContext(context->logicalAndExpression(i),
                                            "is " + Utils::ttos(expList[i].getDataType()) + ", error data type");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (expList[i].getDimension() != 0) {
            ErrorHandler::printErrorContext(context->logicalAndExpression(i),
                                            "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
    }
    return expList[0];
}

std::any SemanticAnalyzer::visitExpression(CACTParser::ExpressionContext *context) {
    if (context->additiveExpression() == nullptr) {
        return ReturnValue(DataType::BOOL, 0, std::vector<int>(), SymbolType::NUM);
    } else {
        return this->visit(context->additiveExpression());
    }
}

std::any SemanticAnalyzer::visitConstantExpression(CACTParser::ConstantExpressionContext *context) {
    if (context->number() == nullptr) {
        if (context->dataType != DataType::BOOL) {
            ErrorHandler::printErrorContext(context,
                                            "Error value type, expect " +
                                            Utils::ttos(context->dataType));
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
    } else {
        auto numberType = std::any_cast<ReturnValue>(this->visit(context->number())).getDataType();
        if (context->dataType != numberType) {
            ErrorHandler::printErrorContext(context,
                                            "Error value type, expect " +
                                            Utils::ttos(context->dataType));
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
    }
    return {};
}

std::any SemanticAnalyzer::visitCondition(CACTParser::ConditionContext *context) {
    auto retVal = std::any_cast<ReturnValue>(this->visit(context->logicalOrExpression()));
    if (retVal.getDataType() != DataType::BOOL) {
        ErrorHandler::printErrorContext(context, "condition is not bool");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    if (retVal.getDimension() != 0) {
        ErrorHandler::printErrorContext(context, "is array");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    return {};
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
    if (!context->arraySize.empty()) { // is array
        if (context->constantExpression() != nullptr) {
            ErrorHandler::printErrorContext(context, "Error init value");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
    }
    if (context->constantExpression() == nullptr) {
        if (arraySize == 0) {
            ErrorHandler::printErrorContext(context, "Error init value");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        for (auto constantInitValue: context->constantInitValue()) {
            constantInitValue->dataType = context->dataType;
            if (constantInitValue->constantExpression() == nullptr) { // type is array
                if (currentSize % subArraySize > 0) {
                    ErrorHandler::printErrorContext(context, "Error init value");
                    throw std::runtime_error(
                            "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
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
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
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
    if (context->compoundStatement() != nullptr) {
        this->visit(context->compoundStatement());
    } else if (context->expressionStatement() != nullptr) {
        this->visit(context->expressionStatement());
    } else if (context->selectionStatement() != nullptr) {
        this->visit(context->selectionStatement());
    } else if (context->iterationStatement() != nullptr) {
        this->visit(context->iterationStatement());
    } else if (context->jumpStatement() != nullptr) {
        this->visit(context->jumpStatement());
    } else {
        ErrorHandler::printErrorContext(context, "statement error");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }

    return {};
}

std::any SemanticAnalyzer::visitCompoundStatement(CACTParser::CompoundStatementContext *context) {
    if (context->thisfuncinfo == nullptr) {
        context->thisblockinfo = currentBlock->addNewBlock();//更新blockinfo
    } else {
        context->thisblockinfo = globalBlock.addNewBlock(context->thisfuncinfo);//根据传进的funcinfo更新blockinfo
    }

    currentBlock = context->thisblockinfo;//更新currentBlock
    this->visit(context->blockItemList());

    /*if(context->blockItemList() != nullptr){
        auto blockitem = context->blockItemList()->lastblockitem;//获得最后一条blockitem,查看其是否满足ifelse returnpath
        if(blockitem->statement() !=nullptr){
            if(blockitem->statement()->selectionStatement() !=nullptr){
                currentBlock->setReturnSign(blockitem->statement()->selectionStatement()->thisblockinfo->getReturnSign() || currentBlock->getReturnSign());
            }
        }
    }//考虑本地的returnsign和if-else的结合，只要有一个满足就可以说returnpath满足*/

    currentBlock = context->thisblockinfo->getParentBlock();//currentBlock回溯
    return {};
}

std::any SemanticAnalyzer::visitBlockItemList(CACTParser::BlockItemListContext *context) {
    for (auto blockitem: context->blockItem()) {
        this->visit(blockitem);

        if(blockitem->statement() !=nullptr){
            if(blockitem->statement()->selectionStatement() !=nullptr){
                currentBlock->setReturnSign((blockitem->statement()->selectionStatement()->thisblockinfo->getReturnSign() && 
                                             blockitem->statement()->selectionStatement()->ifelseType) || 
                                             currentBlock->getReturnSign());
            }
        }
        /*这里因为blockitemlist始终是只为compoundstatement服务的，因此这里的的作用是每次遇到一条blockitem就对当前块，
          也就是compoundstatement对应的块进行returnsign的操作，在compoundstatement的视角中，相当于是顺序的一条路径，
          在这条路径上只要存在一个地方有return(直接的return，或者if-else满足returnpath)那么他就算是满足returnpath*/
        /*这里还要去考虑ifelse有没有，之前在selection你那个地方只用一层就能挖出来，这里要多挖几层*/
        //context->lastblockitem = blockitem;
    }
    return {};
}

std::any SemanticAnalyzer::visitBlockItem(CACTParser::BlockItemContext *context) {
    if (context->declaration() != nullptr) {
        this->visit(context->declaration());
    } else if (context->statement() != nullptr) {
        this->visit(context->statement());
    } else {
        ErrorHandler::printErrorContext(context, "blockitem error");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }

    return {};
}

std::any SemanticAnalyzer::visitExpressionStatement(CACTParser::ExpressionStatementContext *context) {
    if (context->lValue() != nullptr) {
        auto lValue = std::any_cast<ReturnValue>(this->visit(context->lValue()));
        auto expression = std::any_cast<ReturnValue>(this->visit(context->expression()));
        if (lValue.getDataType() != expression.getDataType()) {
            ErrorHandler::printErrorContext(context, "data type error");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (lValue.getDimension() != 0) {
            ErrorHandler::printErrorContext(context->lValue(), "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (expression.getDimension() != 0) {
            ErrorHandler::printErrorContext(context->expression(), "is array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        if (lValue.getSymbolType() == SymbolType::NUM ||
            lValue.getSymbolType() == SymbolType::CONST_ARRAY ||
            lValue.getSymbolType() == SymbolType::CONST) {
            ErrorHandler::printErrorContext(context->lValue(), "is constant");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return {};
    } else {
        return {};
    }
}

std::any SemanticAnalyzer::visitLValue(CACTParser::LValueContext *context) {
    SymbolInfo *symbol = this->currentBlock->lookUpSymbol(context->Identifier()->getText());
    if (symbol == nullptr) {
        ErrorHandler::printErrorContext(context, "use of undeclared identifier");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    if (context->expression().empty()) { // var || const
        if (symbol->getSymbolType() != SymbolType::VAR && symbol->getSymbolType() != SymbolType::CONST) {
            ErrorHandler::printErrorContext(context, "is not variable or constant");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        return ReturnValue(symbol->getDataType(), 0, std::vector<int>(), symbol->getSymbolType());
    } else { // array element
        if (symbol->getSymbolType() != SymbolType::VAR_ARRAY && symbol->getSymbolType() != SymbolType::CONST_ARRAY) {
            ErrorHandler::printErrorContext(context, "is not array");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        for (auto expression: context->expression()) {
            auto retVal = std::any_cast<ReturnValue>(this->visit(expression));
            if (retVal.getDataType() != DataType::INT || retVal.getDimension() != 0) {
                ErrorHandler::printErrorContext(expression, "is invalid array index");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }
        if (context->expression().size() > symbol->getArraySize().size()) {
            ErrorHandler::printErrorContext(context, "too many index");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }
        std::vector<int> arraySize;
        int dimension = symbol->getArraySize().size() - context->expression().size();
        for (int i = context->expression().size(); i < symbol->getArraySize().size(); ++i) {
            arraySize.push_back(symbol->getArraySize()[i]);
        }
        return ReturnValue(symbol->getDataType(), dimension, arraySize, symbol->getSymbolType());
    }
}

std::any SemanticAnalyzer::visitSelectionStatement(CACTParser::SelectionStatementContext *context) {
    context->thisblockinfo = currentBlock->addNewBlock();//更新blockinfo
    if(context->Else() != nullptr)
        context->ifelseType = true;

    currentBlock = context->thisblockinfo;//更新currentBlock
    this->visit(context->condition());


    bool returnFlag = true;//计数，必须保证下面的每个statement都能有返回
    for(auto statement :context->statement()){
        this->visit(statement);
        if(statement->compoundStatement() != nullptr){
            currentBlock->setReturnSign(statement->compoundStatement()->thisblockinfo->getReturnSign());
            //考虑下面的compound是否满足returnpath
        }else if(statement->selectionStatement() != nullptr){
            currentBlock->setReturnSign(statement->selectionStatement()->ifelseType && statement->selectionStatement()->thisblockinfo->getReturnSign());
            //考虑下面既要是if-else Type，同时还得满足returnpath
        }else{
            ;
        }

        returnFlag = returnFlag && currentBlock->getReturnSign();
        currentBlock->setReturnSign(false);//把这个块的returnsign给清空，为下一次循环做准备
    }

    currentBlock->setReturnSign(returnFlag);
    currentBlock = context->thisblockinfo->getParentBlock();//currentBlock回溯
    //return visitChildren(context);
    return{};
}

std::any SemanticAnalyzer::visitIterationStatement(CACTParser::IterationStatementContext *context) {
    context->thisblockinfo = currentBlock->addNewBlock();//更新blockinfo

    currentBlock = context->thisblockinfo;//更新currentBlock
    this->visitChildren(context);
    currentBlock = context->thisblockinfo->getParentBlock();//currentBlock回溯

    //return visitChildren(context);
    return{};
}

std::any SemanticAnalyzer::visitJumpStatement(CACTParser::JumpStatementContext *context) {
    if (context->Return() != nullptr) {
        if (context->expression() != nullptr) {
            auto retVal = std::any_cast<ReturnValue>(this->visit(context->expression()));
            if (retVal.getDimension() != 0) {
                ErrorHandler::printErrorContext(context, "is array");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            if (retVal.getDataType() != this->currentFunc->getDataType()) {
                ErrorHandler::printErrorContext(context, "Error return type");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        } else {
            if (this->currentFunc->getDataType() != DataType::VOID) {
                ErrorHandler::printErrorContext(context, "miss return value");
                throw std::runtime_error(
                        "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
        }

        currentBlock->setReturnSign(true);//当前块returnpath检查没有问题
    } else {
    }
    return {};
}

std::any SemanticAnalyzer::visitCompilationUnit(CACTParser::CompilationUnitContext *context) {
    this->visit(context->translationUnit());//直接访问翻译单元即可
    return {};
}

std::any SemanticAnalyzer::visitTranslationUnit(CACTParser::TranslationUnitContext *context) {
    for (auto externalDeclaration: context->externalDeclaration()) {
        this->visit(externalDeclaration);
        //this->globalBlock.addNewBlock()关于globalblock的操作，在初始化block时就放进去//感觉没有必要维护一个block？
    }
    return {};
}

std::any SemanticAnalyzer::visitExternalDeclaration(CACTParser::ExternalDeclarationContext *context) {
    if (context->declaration() != nullptr) {
        this->visit(context->declaration());//visit子节点
    } else if (context->functionDefinition() != nullptr) {
        this->visit(context->functionDefinition());//visit子节点
    } else {
        ErrorHandler::printErrorContext(context, "externaldeclaration error");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
    return {};
}

std::any SemanticAnalyzer::visitFunctionDefinition(CACTParser::FunctionDefinitionContext *context) {
    std::string returnTypeText = context->functionType()->getText();
    DataType returnType;
    returnType = Utils::stot(returnTypeText);

    context->thisfuncinfo = globalBlock.addNewFunc(context->Identifier()->getText(),
                                                   context->Identifier()->getSymbol()->getLine(), returnType);
    //全局块中的函数表添加，同时获得这个funcdefinition的funcsymbolinfo，为将来的blockinfo初始化做准备
    if (context->functionFParams() != nullptr) {
        if (context->Identifier()->getText() == std::string("main")) {
            ErrorHandler::printErrorContext(context, "main function must be without params");//main函数不能带有参数
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        } else {
            context->functionFParams()->thisfuncinfo = context->thisfuncinfo;
            this->visit(context->functionFParams());//先去访问参数，在将参数都访问完之后可以获得一个完整的函数定义，再去定义blockinfo
            //等待下面的参数层完善这个函数
        }
    }

    currentFunc = context->thisfuncinfo;//更新currentFunc
    context->compoundStatement()->thisfuncinfo = context->thisfuncinfo;

    this->visit(context->compoundStatement());//进入函数体
    //currentFunc->setOp(new IRLabel(ctx->Ident()->getText()));
    context->thisblockinfo = context->compoundStatement()->thisblockinfo;//接收compoundstatement创建的新的blockinfo
    if(context->thisblockinfo->getReturnSign() == false){
        ErrorHandler::printErrorContext(context, "not all path for return");//每个函数退出的时候检查是否可以满足所有路径都有返回
        throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
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
            ErrorHandler::printErrorContext(context, "array dimension error");
            throw std::runtime_error(
                    "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
        }//分析得到paramlist

        context->thisfuncinfo->addParamArray(context->Identifier()->getText(),
                                             context->Identifier()->getSymbol()->getLine(), basicType, param_array,
                                             dimension);
    }

    return {nullptr};
}

void SemanticAnalyzer::analyze() {
    this->visit(this->root);
    if (globalBlock.lookUpFunc("main") == nullptr) {
        ErrorHandler::printErrorMessage("Can not find main");
        throw std::runtime_error(
                "Semantic analysis failed at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
}

std::any SemanticAnalyzer::visitIntegerConstant(CACTParser::IntegerConstantContext *context) {
    return ReturnValue(DataType::INT, 0, std::vector<int>(), SymbolType::NUM);
}

std::any SemanticAnalyzer::visitFloatingConstant(CACTParser::FloatingConstantContext *context) {
    unsigned length = context->FloatingConstant()->getText().size();
    char suffix = context->FloatingConstant()->getText()[length - 1];
    if (suffix == 'f' || suffix == 'F') {
        return ReturnValue(DataType::FLOAT, 0, std::vector<int>(), SymbolType::NUM);
    } else {
        return ReturnValue(DataType::DOUBLE, 0, std::vector<int>(), SymbolType::NUM);
    }
}
