#ifndef COMPILER_IMEMORY_H
#define COMPILER_IMEMORY_H


#include "IRInstruction.h"
class IRPointerType;

class IRAllocationInst : public IRInstruction {
protected:
    IRAllocationInst(IRType *Ty, IRValue *ArraySize, unsigned iTy,
                     const std::string &Name = "", IRBasicBlock *parent = nullptr);

public:
    bool isArrayAllocation() const;
    inline IRValue *getArraySize() { return Operands[0]; }
    inline const IRPointerType *getType() const {
        return (const IRPointerType *) IRInstruction::getType();
    }
    const IRType *getAllocatedType() const;
    virtual IRInstruction *clone() const = 0;

    static inline bool classof(const IRAllocationInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return I->getOpcode() == IRInstruction::Alloca;
    }
};

class IRAllocaInst : public IRAllocationInst {
    IRAllocaInst(const IRAllocaInst &);

public:
    explicit IRAllocaInst(IRType *Ty, IRValue *ArraySize = nullptr, const std::string &Name = "",
                          IRBasicBlock *parent = nullptr)
        : IRAllocationInst(Ty, ArraySize, Alloca, Name, parent) {}

    IRInstruction *clone() const override {
        return new IRAllocaInst(*this);
    }

    static inline bool classof(const IRAllocaInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return (I->getOpcode() == IRInstruction::Alloca);
    }
};

class IRLoadInst : public IRInstruction {
    IRLoadInst(const IRLoadInst &LI) : IRInstruction(LI.getType(), Load) {
        Volatile = LI.isVolatile();
        Operands.reserve(1);
        Operands.emplace_back(LI.Operands[0], this);
    }
    bool Volatile;// True if this is a volatile load
public:
    IRLoadInst(IRValue *Ptr, const std::string &Name, IRBasicBlock *parent);
    explicit IRLoadInst(IRValue *Ptr, const std::string &Name = "", bool isVolatile = false,
                        IRBasicBlock *parent = nullptr);
    IRInstruction *clone() const override { return new IRLoadInst(*this); }

    /******是否每次要从内存中取值&&存值******/
    bool isVolatile() const { return Volatile; }
    void setVolatile(bool V) { Volatile = V; }
    bool mayWriteToMemory() const override { return isVolatile(); }

    /******load&&store中与存储地址相关的operand******/
    IRValue *getPointerOperand() { return getOperand(0); }
    static unsigned getPointerOperandIndex() { return 0U; }

    static inline bool classof(const IRLoadInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return I->getOpcode() == IRInstruction::Load;
    }
};

class IRStoreInst : public IRInstruction {
    IRStoreInst(const IRStoreInst &SI) : IRInstruction(SI.getType(), Store) {
        Volatile = SI.isVolatile();
        Operands.reserve(2);
        Operands.emplace_back(SI.Operands[0], this);
        Operands.emplace_back(SI.Operands[1], this);
    }
    bool Volatile;// True if this is a volatile store
public:
    IRStoreInst(IRValue *Val, IRValue *Ptr, IRBasicBlock *InsertBefore);
    IRStoreInst(IRValue *Val, IRValue *Ptr, bool isVolatile = false,
                IRBasicBlock *parent = nullptr);
    virtual IRInstruction *clone() const { return new IRStoreInst(*this); }

    /******是否每次要从内存中取值&&存值******/
    bool isVolatile() const { return Volatile; }
    void setVolatile(bool V) { Volatile = V; }
    virtual bool mayWriteToMemory() const { return true; }

    /******load&&store中与存储地址相关的operand******/
    IRValue *getPointerOperand() { return getOperand(1); }
    static unsigned getPointerOperandIndex() { return 1U; }

    static inline bool classof(const IRStoreInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return I->getOpcode() == IRInstruction::Store;
    }
};

#endif//COMPILER_IMEMORY_H
