#include "BranchInst.h"
#include "IR/IRType.h"
#include "Value.h"

namespace RISCV {
    BranchInst::BranchInst(unsigned int iType, BasicBlock *dest, Value *op1, Value *op2, BasicBlock *parent)
            : Instruction(IRType::VoidTy, iType, parent) {
        Operands.push_back(op1);
        if (op2) {
            Operands.push_back(op2);
        }
        Operands.push_back(new Value(dest));
    }

    void BranchInst::print(std::ostream &O) const {
        switch (iType) {
            case Beq:
                O << "beq ";
            case Beqz:
                O << "beqz ";
            case Bge:
                O << "";
        }
    }
} // RISCV