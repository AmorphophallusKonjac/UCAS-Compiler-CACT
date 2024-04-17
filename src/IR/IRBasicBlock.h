#ifndef COMPILER_IRBASICBLOCK_H
#define COMPILER_IRBASICBLOCK_H
#pragma once

#include "IRFunction.h"
#include "IRInstruction.h"
#include "IRValue.h"

class IRTerminatorInst;


class IRBasicBlock : public IRValue {
private:
    std::vector<IRInstruction *> InstList; 

    IRFunction *parent;

public:
    explicit IRBasicBlock(const std::string &Name = "", IRFunction *Parent = 0);
    IRBasicBlock(const std::string &Name, IRBasicBlock *InsertBefore);
    ~IRBasicBlock() override = default;

    void setParent(IRFunction *parent);
    IRFunction *getParent() { return parent; }

    /******IRBasicBlock获得指令列表******/
    IRTerminatorInst *getTerminator();//获得终止指令
    std::vector<IRInstruction *> &getInstList() { return InstList; }

    /******IRBasicBlock的打印方法******/
    void printPrefixName(std::ostream &OS) const override;
    virtual void print(std::ostream &OS) const;

    /******向InstList中添加instruction******/
    void addInstruction(IRInstruction *inst);

    /******classof方法，判断从父类下来的子类是不是对应的IRBasicBlock类******/
    static inline bool classof(const IRBasicBlock *BB) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == IRValue::BasicBlockVal;
    }
};


#endif//COMPILER_IRBASICBLOCK_H
