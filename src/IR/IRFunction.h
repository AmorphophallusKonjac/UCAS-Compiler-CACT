#ifndef COMPILER_IRFUNCTION_H
#define COMPILER_IRFUNCTION_H
#pragma once

#include "IRArgument.h"
#include "IRDerivedTypes.h"
#include "IRGlobalValue.h"
#include "IRBasicBlock.h"

class IRFunctionType;
class FuncSymbolInfo;

class IRFunction : public IRGlobalValue {
public:
    enum FuncTy{
        Declared,
        UnDeclared,
    };

private:
    std::vector<IRArgument *> ArgumentList;
    std::vector<IRBasicBlock *> BasicBlocks;
    IRModule *Parent;
    unsigned IRSymbolCount = 0;
    FuncTy fTy;

    void setParent(IRModule *parent);

public:
    IRFunction(IRFunctionType *Ty, IRGlobalValue::LinkageTypes Linkage, 
            const std::string &N, IRModule *M, FuncTy fty);
    ~IRFunction() = default;

//    const IRType *getReturnType() const;            // Return the type of the ret val
    const FuncTy  getFuntTy() { return fTy; };

    /******IRFunction继承IRvalue，获得它的IRtype指针******/
    IRFunctionType *getFunctionType() const;        // Return the FunctionType for me

    /******函数的arg和basicblock******/
    std::vector<IRArgument *> &getArgumentList() { return ArgumentList; }
    std::vector<IRBasicBlock *> &getBasicBlockList() { return BasicBlocks; }

    void addArgument(IRArgument * arg);
    void addBasicBlock(IRBasicBlock * block);

    IRBasicBlock *getEntryBlock() { return BasicBlocks.front(); }

    /******IRFunction的print方法******/
    void printPrefixName(std::ostream &OS) const override;
    void Funcprint(std::ostream &OS) const;
    void print(std::ostream &OS) const override;

    /******classof方法，判断从父类下来的子类是不是对应的IRFunction类******/
    static inline bool classof(const IRFunction *) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == IRValue::FunctionVal;
    }

    /******每一个函数维护自己这个函数内记录了多少个IRSymbol******/
    void addCount(){ IRSymbolCount++; }
    unsigned getCount(){ return IRSymbolCount; }
    void setCount(unsigned Count){ IRSymbolCount = Count; }
};


#endif//COMPILER_IRFUNCTION_H
