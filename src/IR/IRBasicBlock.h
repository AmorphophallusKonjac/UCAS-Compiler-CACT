#ifndef COMPILER_IRBASICBLOCK_H
#define COMPILER_IRBASICBLOCK_H


#include "IRFunction.h"
#include "IRInstruction.h"
#include "IRValue.h"

class IRTerminatorInst;


class IRBasicBlock : public IRValue {
private:
    std::vector<IRInstruction *> InstList;

    void setParent(IRFunction *parent);

    IRFunction *parent;

public:
    explicit IRBasicBlock(const std::string &Name = "", IRFunction *Parent = 0);

    IRBasicBlock(const std::string &Name, IRBasicBlock *InsertBefore);

    ~IRBasicBlock() override = default;

    IRFunction *getParent() { return parent; }

    IRTerminatorInst *getTerminator();

    std::vector<IRInstruction *> &getInstList() { return InstList; }

    virtual void print(std::ostream &OS) const;

    void addInstruction(IRInstruction *inst);

    static inline bool classof(const IRBasicBlock *BB) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == IRValue::BasicBlockVal;
    }
};


#endif//COMPILER_IRBASICBLOCK_H
