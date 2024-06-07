#include "BranchInst.h"
#include "IR/IRType.h"

namespace RISCV {
    BranchInst::BranchInst(unsigned int iType, BasicBlock *dest, Value *op1, Value *op2, BasicBlock *parent)
            : Instruction(IRType::VoidTy, iType, parent) {

    }
} // RISCV