#ifndef COMPILER_SYMBOLTABLE_H
#define COMPILER_SYMBOLTABLE_H

#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "utils/CACT.h"
#include "utils/ErrorHandler.h"

std::string ToString(DataType x);

int SizeOfDataType(DataType x);

class IROperand;

/***********符号表***********/
class SymbolInfo {
private:
    int line;//表示是第几行声明的变量，函数，或者块
    std::string name;
    IROperand *operand;

public:
    std::string getName() { return name; };

    int getline() { return line; };

    void setOp(IROperand *op);

    IROperand *getOp();

    virtual DataType getDataType() = 0;

    virtual int getArrayLength() = 0;

    virtual std::vector<int> getArraySize() = 0;

    virtual SymbolType getSymbolType() = 0;
    //datatype:VOID,BOOL,INT,FLOAT,DOUBLE
    //arraysize:针对数组而言
    //symboltype:CONST,VAR,CONST_ARRAY,VAR_ARRAY,FUNC

    SymbolInfo(const std::string &name, int line);

    ~SymbolInfo(){};
};

class initValue{
private:
    std::vector<int>    intArray;
    std::vector<bool>   boolArray;
    std::vector<double> doubleArray;
    std::vector<float>  floatArray;

public:

    void setInitValue(int    value){ intArray.push_back(value);   };
    void setInitValue(bool   value){ boolArray.push_back(value);   };
    void setInitValue(double value){ doubleArray.push_back(value); };
    void setInitValue(float  value){ floatArray.push_back(value);  };
    void setZero(DataType datatype){
        switch (datatype) {
            case INT:
                intArray.push_back(0);
                return;
            case BOOL:
                boolArray.push_back(0);
                return;
            case DOUBLE:
                doubleArray.push_back(0);
                return;
            case FLOAT:
                floatArray.push_back(0);
                return;
            default:
                return;//如果匹配不成功的话就返回一个空指针
        }
    }
    int  getCurrentArraySize(DataType datatype){
        switch (datatype) {
            case INT:
                return intArray.size();
            case BOOL:
                return boolArray.size();
            case DOUBLE:
                return doubleArray.size();
            case FLOAT:
                return floatArray.size();
            default:
                return -1;//如果匹配不成功的话就返回一个空指针
        }
    }

    std::any getInitValue(DataType datatype, std::vector<int> indexArray){ 
        
        int index = 0;
        std::vector<int>::iterator element = indexArray.begin();
        while (element != indexArray.end()) {
            index += std::accumulate(element, indexArray.end(), 1, std::multiplies<int>());
            element++;
        }

        switch (datatype) {
            case INT:
                return intArray[index];
            case BOOL:
                return boolArray[index];
            case DOUBLE:
                return doubleArray[index];
            case FLOAT:
                return floatArray[index];
            default:
                return nullptr;//如果匹配不成功的话就返回一个空指针
        }
    };

    std::any getInitValue(DataType datatype){
        switch (datatype) {
            case INT:
                return intArray[0];
            case BOOL:
                return boolArray[0];
            case DOUBLE:
                return doubleArray[0];
            case FLOAT:
                return floatArray[0];
            default:
                return nullptr;//如果匹配不成功的话就返回一个空指针
        }
    };
};

/***********常量变量数组符号表***********/
class ConstVarArraySymbolInfo : public SymbolInfo, public initValue {
private:
    DataType dataType;
    int global = 0;//代表是否是全局变量

public: 
    DataType getDataType() { return dataType; }

    int getGlobal() { return global; }

    virtual int getArrayLength() = 0;
    virtual std::vector<int> getArraySize() = 0;
    virtual SymbolType getSymbolType() = 0;

    ConstVarArraySymbolInfo(const std::string &name, int line, DataType dataType, int global);

    ~ConstVarArraySymbolInfo(){};
};


/***********常量符号表***********/
class ConstSymbolInfo : public ConstVarArraySymbolInfo {
public:
    int getArrayLength() { return -1; }//不是数组

    std::vector<int> getArraySize() override { return std::vector<int>(); }

    SymbolType getSymbolType() { return SymbolType::CONST; }

    ConstSymbolInfo(const std::string &name, int line, DataType dataType, int global);

    ~ConstSymbolInfo(){};
};


/***********变量符号表***********/
class VarSymbolInfo : public ConstVarArraySymbolInfo {
public:
    int getArrayLength() { return -1; }

    std::vector<int> getArraySize() override { return std::vector<int>(); }

    SymbolType getSymbolType() { return SymbolType::VAR; }

    VarSymbolInfo(const std::string &name, int line, DataType dataType, int global);

    ~VarSymbolInfo(){};
};


/***********常量数组符号表***********/
class ConstArraySymbolInfo : public ConstVarArraySymbolInfo {
private:
    std::vector<int> arraySize;
    int dimension;

public:
    std::vector<int> getArraySize() override { return arraySize; }

    int getDimension() { return dimension; }

    int getArrayLength() { return std::accumulate(arraySize.begin(), arraySize.end(), 1, std::multiplies<int>()); }

    SymbolType getSymbolType() { return SymbolType::CONST_ARRAY; }

    ConstArraySymbolInfo(const std::string &name, int line, DataType dataType, int global,
                         const std::vector<int> arraySize, int dimension);

    ~ConstArraySymbolInfo(){};
};


/***********变量数组符号表***********/
class VarArraySymbolInfo : public ConstVarArraySymbolInfo {
private:
    std::vector<int> arraySize;
    int dimension;

public:
    int getDimension() { return dimension; }

    int getArrayLength() { return std::accumulate(arraySize.begin(), arraySize.end(), 1, std::multiplies<int>()); }

    std::vector<int> getArraySize() override { return arraySize; }

    SymbolType getSymbolType() { return SymbolType::VAR_ARRAY; }

    VarArraySymbolInfo(const std::string &name, int line, DataType dataType, int global,
                       const std::vector<int> arraySize, int dimension);

    ~VarArraySymbolInfo(){};
};

/*相对于徐泽凡学长做的改动：
  1)对于符号表加入了行号的属性
  2)对于常量变量数组符号加入了是否为全局变量的属性
  3)多做了一级继承，以上四个类全部继承自ConstVarArraySymbolInfo*/


class BlockInfo;//为了funcsymbolinfo的定义
class GlobalBlock;


class FuncSymbolInfo : public SymbolInfo {
private:
    int stack_size = 0;//函数需要栈的大小
    DataType returnType;
    std::vector<SymbolInfo *> paramList;
    BlockInfo *baseblock;//函数的基本块
public:
    virtual int getStackSize() { return stack_size; }

    virtual DataType getDataType() { return returnType; }

    virtual int getArrayLength() { return paramList.size(); }//函数参数列表的数组大小

    std::vector<int> getArraySize() override { return std::vector<int>(); }

    virtual SymbolType getSymbolType() { return SymbolType::FUNC; }


    std::vector<SymbolInfo *> getparamList() { return paramList; }

    int getparamNum() { return paramList.size(); }

    SymbolInfo *addParamVar(const std::string &name, int line, DataType dataType);

    SymbolInfo *addParamArray(const std::string &name, int line, DataType dataType, const std::vector<int> arraySize,
                              int dimension);

    // FuncSymbolInfo(const std::string & name, DataType returnType, int paramNum);
    FuncSymbolInfo(const std::string &name, int line, DataType returnType);

    ~FuncSymbolInfo() {
        for (SymbolInfo *symbol: paramList) { delete symbol; };
    };
}; 


/***********各种table***********/
class FuncTable {
public:
    std::map<std::string, FuncSymbolInfo *> funcList;
    std::string curFunc = "$";
    int stackFunc_size = 0;

    ~FuncTable() {
        for (auto it = funcList.begin(); it != funcList.end(); ++it) { delete it->second; }
        funcList.clear();
    };
};

class SymbolTable {
public:
    std::map<std::string, SymbolInfo *> symbolList;
    std::string curSymbol = "$";
    int stackSymbol_size = 0;

    ~SymbolTable() {
        for (auto it = symbolList.begin(); it != symbolList.end(); ++it) { delete it->second; }
        symbolList.clear();
    };
};

class BlockTable {
public:
    std::vector<BlockInfo *> blockList;
    int stackBlock_size = 0;

    ~BlockTable() {
        for (BlockInfo *block: blockList) { delete block; };
    };
};


/***********BlockInfo与Globalblock,block是记录符号表，函数表，块表的基本单位***********/
//对于块而言，这里不再强调它的line
class BlockInfo {
protected:
    BlockInfo *parentBlock;
    FuncSymbolInfo *belongTo = nullptr;//块属于某一个函数

    SymbolTable symbolTable;
    BlockTable blockTable;
    bool returnsign = false;//认定这个块是否满足returnpath都能满足
    //FuncTable funcTable;
    //这里我认为对于一个块而言只需要考虑他的符号表和他的subblock，函数表,对于函数表我的想法是可以做一个全局的，

public:
    BlockInfo *getParentBlock() { return parentBlock; }
    bool getReturnSign() { return returnsign; };
    void setReturnSign(bool returnSign) { returnsign = returnSign; }

    SymbolInfo *lookUpSymbol(std::string symbolName);

    virtual ConstSymbolInfo *addNewConst(const std::string &name, int line, DataType dataType);

    virtual VarSymbolInfo *addNewVar(const std::string &name, int line, DataType dataType);

    virtual ConstArraySymbolInfo *
    addNewConstArray(const std::string &name, int line, DataType dataType, const std::vector<int> arraySize,
                     int dimension);

    virtual VarArraySymbolInfo *
    addNewVarArray(const std::string &name, int line, DataType dataType, const std::vector<int> arraySize,
                   int dimension);

    BlockInfo *addNewBlock();

    BlockInfo *addNewBlock(FuncSymbolInfo *belongTo);
    //两个addNewBlock，主要是看这个block是不是函数
    //如果说是一个函数，那么在new一个新块的时候，自然的这个函数的形参就要加到这个块的符号表里面去

    BlockInfo(BlockInfo *parentBlock);

    BlockInfo(BlockInfo *parentBlock, FuncSymbolInfo *belongTo, const std::vector<SymbolInfo *> &paramList);

    ~BlockInfo(){};
    //如果说是直接隶属于函数的块，则需要记录belongto,同时所有函数的形参都作为这个块的符号表而存在
};

class GlobalBlock : public BlockInfo {
private:
    FuncTable funcTable;

public:
    FuncSymbolInfo *lookUpFunc(std::string symbolName);

    FuncSymbolInfo *addNewFunc(const std::string &name, int line, DataType returnType);

    ConstSymbolInfo *addNewConst(const std::string &name, int line, DataType dataType) override;

    VarSymbolInfo *addNewVar(const std::string &name, int line, DataType dataType) override;

    ConstArraySymbolInfo *
    addNewConstArray(const std::string &name, int line, DataType dataType, const std::vector<int> arraySize,
                     int dimension) override;

    VarArraySymbolInfo *
    addNewVarArray(const std::string &name, int line, DataType dataType, const std::vector<int> arraySize,
                   int dimension) override;

    void initIOFunction();

    GlobalBlock();

    ~GlobalBlock() = default;
};

/*在初始化一个块的时候，本来应该这个块的符号表和子块都是空的·*/

#endif