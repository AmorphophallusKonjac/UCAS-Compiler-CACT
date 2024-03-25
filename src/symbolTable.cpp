#include "symbolTable.h"
#include <iostream>

std::string ToString(DataType x) {
    if (x == VOID) {
        return "void";
    } else if (x == BOOL) {
        return "bool";
    } else if (x == INT) {
        return "int";
    } else if (x == FLOAT) {
        return "float";
    } else if (x == DOUBLE) {
        return "double";
    }
    return "unknown";
}

int SizeOfDataType(DataType x) {
    if (x == BOOL) {
        return 4;
    } else if (x == INT) {
        return 4;
    } else if (x == FLOAT) {
        return 4;
    } else if (x == DOUBLE) {
        return 8;
    }
    return 0;
}


/***********符号表***********/
SymbolInfo::SymbolInfo(const std::string & name, int line)
: name(name), operand(nullptr), line(line) {}

void SymbolInfo::setOp(IROperand * op) {
    operand = op;
}

IROperand * SymbolInfo::getOp() {
    if (!operand) {
        throw std::runtime_error("try to get null op");
    }
    return operand;
}


/***********常量变量数组符号表(init函数)***********/
ConstVarArraySymbolInfo::ConstVarArraySymbolInfo(const std::string & name, int line, DataType dataType, int global)
: SymbolInfo(name, line), dataType(dataType), global(global) { }

ConstSymbolInfo::ConstSymbolInfo(const std::string & name, int line, DataType dataType, int global)
: ConstVarArraySymbolInfo(name, line, dataType, global) { }


VarSymbolInfo::VarSymbolInfo(const std::string & name, int line, DataType dataType, int global)
: ConstVarArraySymbolInfo(name, line, dataType, global) { }


ConstArraySymbolInfo::ConstArraySymbolInfo(const std::string & name, int line, DataType dataType, int global, const std::vector <int> arraySize, int dimension)
: ConstVarArraySymbolInfo(name, line, dataType, global), arraySize(arraySize), dimension(dimension) { }


VarArraySymbolInfo::VarArraySymbolInfo(const std::string & name, int line, DataType dataType, int global, const std::vector <int> arraySize, int dimension)
: ConstVarArraySymbolInfo(name, line, dataType, global), arraySize(arraySize), dimension(dimension) { }




/***********FuncSymbolInfo***********/
FuncSymbolInfo::FuncSymbolInfo(const std::string & name, int line, DataType returnType)
: SymbolInfo(name, line), returnType(returnType), baseblock(nullptr) { }

SymbolInfo * FuncSymbolInfo::addParamVar(const std::string & name, int line, DataType dataType) {
    VarSymbolInfo * newParam = new VarSymbolInfo(name, line, dataType, 0);//函数的形参必然不是全局变量  
    paramList.push_back(newParam);//函数形参压栈
    return newParam;
}

SymbolInfo * FuncSymbolInfo::addParamArray(const std::string & name, int line, DataType dataType, const std::vector <int> arraySize, int dimension) {
    VarArraySymbolInfo * newParam = new VarArraySymbolInfo(name, line, dataType, 0, arraySize, dimension);//函数形参数组的全局属性和arraysize都是0
    paramList.push_back(newParam);
    return newParam;
}



/***********BlockInfo***********/

/***********BlockInfo(init函数)***********/
BlockInfo::BlockInfo(BlockInfo * parentBlock): parentBlock(parentBlock), belongTo(nullptr) { }
BlockInfo::BlockInfo(BlockInfo * parentBlock, FuncSymbolInfo * belongTo, const std::vector < SymbolInfo * > & paramList)
: parentBlock(parentBlock), belongTo(belongTo) {
    for (SymbolInfo * one_param : paramList) {
        if (symbolTable.symbolList.count(one_param->getName()) > 0) {
            throw std::runtime_error("dupliacte name of parameters");
            return;
        }

        //对symboltable进行操作
        symbolTable.symbolList[one_param->getName()] = one_param;
        symbolTable.stackSymbol_size += SizeOfDataType(one_param->getDataType())*
                                        ((one_param->getArraySize() == -1) ? 1 : one_param->getArraySize());//看看是否是数组，是数组那么栈要多加一些
        symbolTable.curSymbol = one_param->getName();
    }
}

/***********SymbolTable相关***********/

/***********在Block中查找符号表***********/
SymbolInfo * BlockInfo::lookUpSymbol(std::string symbolName) {
    if (symbolTable.symbolList.count(symbolName) == 1) {
        return symbolTable.symbolList[symbolName];
    } else if (parentBlock != nullptr) {
        return parentBlock->lookUpSymbol(symbolName);
    }
    return nullptr;
}//一层层往外递归查找符号
//注意这里的lookUpSymbol是不会局限于一个块的，因此后面检查duplicate的时候不能直接调用这个函数

/***********在Block中添加各种符号***********/
ConstSymbolInfo * BlockInfo::addNewConst(const std::string & name, int line, DataType dataType) {
    //symbol符号表添加
    if (symbolTable.symbolList.count(name) > 0) {//这里注意，我还会去全局的函数表里面寻找函数名，任何块内定义的变量名都不能和全局的函数名相同//不用
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    ConstSymbolInfo * newSymbol = new ConstSymbolInfo(name, line, dataType, 0);

    //对symboltable进行操作

    return newSymbol;
}

VarSymbolInfo * BlockInfo::addNewVar(const std::string & name, int line, DataType dataType) {
    if (symbolTable.symbolList.count(name) > 0) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    VarSymbolInfo * newSymbol = new VarSymbolInfo(name, line, dataType, 0);

    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType);
    symbolTable.curSymbol = name;

    return newSymbol;
}

ConstArraySymbolInfo * BlockInfo::addNewConstArray(const std::string & name, int line, DataType dataType, const std::vector <int> arraySize, int dimension) {
    if (symbolTable.symbolList.count(name) > 0) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    ConstArraySymbolInfo * newSymbol = new ConstArraySymbolInfo(name, line, dataType, 0, arraySize, dimension);

    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType)*(newSymbol->getArraySize());
    symbolTable.curSymbol = name;

    return newSymbol;
}                  

VarArraySymbolInfo * BlockInfo::addNewVarArray(const std::string & name, int line, DataType dataType, const std::vector <int> arraySize, int dimension) {
    if (symbolTable.symbolList.count(name) > 0) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    VarArraySymbolInfo * newSymbol = new VarArraySymbolInfo(name, line, dataType, 0, arraySize, dimension);

    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType)*(newSymbol->getArraySize());
    symbolTable.curSymbol = name;

    return newSymbol;
}


/***********BlockTable相关***********/
//一般不会去查找block
BlockInfo * BlockInfo::addNewBlock(FuncSymbolInfo * belongTo) {
    BlockInfo * newBlock = new BlockInfo(this, belongTo, belongTo->getparamList());

    //对blocktable进行操作
    blockTable.blockList.push_back(newBlock);
    blockTable.stackBlock_size++;

    return newBlock;
}
BlockInfo * BlockInfo::addNewBlock() {
    BlockInfo * newBlock = new BlockInfo(this);

    //对blocktable进行操作
    blockTable.blockList.push_back(newBlock);
    blockTable.stackBlock_size++;

    return newBlock;
}



/***********GlobalBlock***********/

GlobalBlock::GlobalBlock()
: BlockInfo(nullptr) {};

/***********在GlobalBlock中添加各种符号***********/
//注意这里的函数与blockInfo不是覆写的关系，因为添加常量或者变量总需要查找全局的函数表以防同名，这里直接查自己即可
ConstSymbolInfo * GlobalBlock::addNewConst(const std::string & name, int line, DataType dataType) {
    if (symbolTable.symbolList.count(name) > 0 || lookUpFunc(name) != nullptr) {//这里注意，我还会去全局的函数表里面寻找函数名，全局变量不能和全局的函数名相同
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    ConstSymbolInfo * newSymbol = new ConstSymbolInfo(name, line, dataType, 0);
    
    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType);
    symbolTable.curSymbol = name;

    return newSymbol;
}

VarSymbolInfo * GlobalBlock::addNewVar(const std::string & name, int line, DataType dataType) {
    if (symbolTable.symbolList.count(name) > 0 || lookUpFunc(name) != nullptr) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    VarSymbolInfo * newSymbol = new VarSymbolInfo(name, line, dataType, 0);
    
    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType);
    symbolTable.curSymbol = name;

    return newSymbol;
}

ConstArraySymbolInfo * GlobalBlock::addNewConstArray(const std::string & name, int line, DataType dataType, const std::vector <int> arraySize, int dimension) {
    if (symbolTable.symbolList.count(name) > 0 || lookUpFunc(name) != nullptr) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    ConstArraySymbolInfo * newSymbol = new ConstArraySymbolInfo(name, line, dataType, 0, arraySize, dimension);
    
    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType)*(newSymbol->getArraySize());
    symbolTable.curSymbol = name;

    return newSymbol;
}                  

VarArraySymbolInfo * GlobalBlock::addNewVarArray(const std::string & name, int line, DataType dataType, const std::vector <int> arraySize, int dimension) {
    if (symbolTable.symbolList.count(name) > 0 || lookUpFunc(name) != nullptr) {
        throw std::runtime_error("duplicate name of symbol");
        return nullptr;
    }
    VarArraySymbolInfo * newSymbol = new VarArraySymbolInfo(name, line, dataType, 0, arraySize, dimension);
    
    //对symboltable进行操作
    symbolTable.symbolList[name] = newSymbol;
    symbolTable.stackSymbol_size += SizeOfDataType(dataType)*(newSymbol->getArraySize());
    symbolTable.curSymbol = name;

    return newSymbol;
}


/***********FuncTable相关***********/
//由于所有的函数都是全局的，因此如果要查找函数表，必须去全局的块里面进行查询，在其他的块里面进行查询都是无效的
FuncSymbolInfo * GlobalBlock::lookUpFunc(std::string symbolName) {
    if (funcTable.funcList.count(symbolName) == 1) {
        if (funcTable.funcList[symbolName]->getSymbolType() == SymbolType::FUNC) {
            return funcTable.funcList[symbolName];
        } else {
            throw std::runtime_error("this is not a func");
            return nullptr;
        }
    }
    return nullptr;
}

FuncSymbolInfo * GlobalBlock::addNewFunc(const std::string & name, int line, DataType returnType) {
    //首先在global中加入函数
    if (funcTable.funcList.count(name) > 0) {
        throw std::runtime_error("duplicate name of function");
        return nullptr;
    }
    FuncSymbolInfo * newFunc = new FuncSymbolInfo(name, line, returnType);

    //对functable进行操作
    funcTable.funcList[name] = newFunc;
    funcTable.stackFunc_size++;
    funcTable.curFunc = name;

    //然后在global中加入函数的block
    addNewBlock(newFunc);

    return newFunc;
}