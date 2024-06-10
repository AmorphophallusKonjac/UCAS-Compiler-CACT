#include "MoveInst.h"
#include "IR/IRType.h"
#include "Value.h"

namespace RISCV {
    MoveInst::MoveInst(unsigned int iType, IRType *ty, Value *dest, Value *src, BasicBlock *parent)
            : Instruction(IRType::VoidTy, iType, parent), ty(ty) {
        Operands.push_back(dest);
        Operands.push_back(src);
    }

    void MoveInst::print(std::ostream &O) const {
        if (ty == IRType::IntTy) {
            O << "mv ";
        } else if (ty == IRType::FloatTy) {
            O << "fmv.s ";
        } else if (ty == IRType::DoubleTy) {
            O << "fmv.d ";
        } else
            assert(0 && "Error ty");
        Operands[0]->print(O);
        O << ", ";
        Operands[1]->print(O);
        O << std::endl;
    }
} // RISCV