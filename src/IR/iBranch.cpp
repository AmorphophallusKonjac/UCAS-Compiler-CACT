#include "IRBasicBlock.h"
#include "IRType.h"
#include "iTerminators.h"

IRBranchInst::IRBranchInst(IRBasicBlock *IfTrue, IRBasicBlock *IfFalse, IRValue *cond, IRInstruction *InsertBefore)
    : IRTerminatorInst(IRInstruction::Br, InsertBefore) {
    Operands.reserve(IfFalse ? 3 : 1);
    Operands.emplace_back(IfTrue, this);
    if (IfFalse) {
        Operands.emplace_back(IfFalse, this);
        Operands.emplace_back(cond, this);
    }
}
IRBranchInst::IRBranchInst(IRBasicBlock *IfTrue, IRInstruction *InsertBefore)
    : IRTerminatorInst(IRInstruction::Br, InsertBefore) {
    Operands.reserve(1);
    Operands.emplace_back(IfTrue, this);
}

IRBranchInst::IRBranchInst(const IRBranchInst &BI) : IRTerminatorInst(IRInstruction::Br) {
    Operands.reserve(BI.Operands.size());
    Operands.emplace_back(BI.Operands[0].val, this);
    if (BI.Operands.size() != 1) {
        assert(BI.Operands.size() == 3 && "BR can have 1 or 3 operands!");
        Operands.emplace_back(BI.Operands[1].val, this);
        Operands.emplace_back(BI.Operands[2].val, this);
    }
}
