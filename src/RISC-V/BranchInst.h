#ifndef COMPILER_BRANCHINST_H
#define COMPILER_BRANCHINST_H

#include "Instruction.h"

namespace RISCV {

    class BranchInst : public Instruction {
    public:
        BranchInst(unsigned iType, BasicBlock *dest, Value *op1, Value *op2, BasicBlock *parent = nullptr);
    };

} // RISCV

#endif //COMPILER_BRANCHINST_H
