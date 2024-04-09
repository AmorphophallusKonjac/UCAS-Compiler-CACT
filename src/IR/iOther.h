#ifndef COMPILER_IOTHER_H
#define COMPILER_IOTHER_H

#include "InstrTypes.h"

class IRCallInst : public IRInstruction {
    IRCallInst(const IRCallInst &CI);

public:
    IRCallInst(IRValue *F, const std::vector<IRValue *> &Par,
               const std::string &Name = "", IRInstruction *InsertBefore = 0);

    // Alternate CallInst ctors w/ no actuals & one actual, respectively.
    IRCallInst(IRValue *F, const std::string &Name = "",
               IRInstruction *InsertBefore = 0);
    IRCallInst(IRValue *F, IRValue *Actual, const std::string &Name = "",
               IRInstruction *InsertBefore = 0);

    virtual IRInstruction *clone() const { return new IRCallInst(*this); }
    bool mayWriteToMemory() const { return true; }

    IRFunction *getCalledFunction() {
        return dynamic_cast<IRFunction *>(Operands[0].val);
    }

    // getCalledValue - Get a pointer to a method that is invoked by this inst.
    inline IRValue *getCalledValue() { return Operands[0].val; }

    // Methods for support type inquiry through isa, cast, and dyn_cast:
    static inline bool classof(const IRCallInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return I->getOpcode() == IRInstruction::Call;
    }
};

class ShiftInst : public IRInstruction {
    ShiftInst(const ShiftInst &SI) : IRInstruction(SI.getType(), SI.getOpcode()) {
        Operands.reserve(2);
        Operands.emplace_back(SI.Operands[0].val, this);
        Operands.emplace_back(SI.Operands[1].val, this);
    }

public:
    ShiftInst(OtherOps Opcode, IRValue *S, IRValue *SA, const std::string &Name = "",
              IRInstruction *InsertBefore = nullptr)
        : IRInstruction(S->getType(), Opcode, Name, InsertBefore) {
        assert((Opcode == Shl || Opcode == Shr) && "ShiftInst Opcode invalid!");
        Operands.reserve(2);
        Operands.emplace_back(S, this);
        Operands.emplace_back(SA, this);
    }

    OtherOps getOpcode() const { return (OtherOps) IRInstruction::getOpcode(); }

    IRInstruction *clone() const override { return new ShiftInst(*this); }

    // Methods for support type inquiry through isa, cast, and dyn_cast:
    static inline bool classof(const ShiftInst *) { return true; }
    static inline bool classof(const IRInstruction *I) {
        return (I->getOpcode() == IRInstruction::Shr) |
               (I->getOpcode() == IRInstruction::Shl);
    }
};


#endif//COMPILER_IOTHER_H
