#include "StoreInst.h"
#include "IR/IRType.h"

namespace RISCV {
    StoreInst::StoreInst(unsigned int iType, Value *rd, Pointer *ptr, BasicBlock *parent)
            : Instruction(IRType::VoidTy, iType, parent) {
        Operands.push_back(rd);
        Operands.push_back(ptr);
    }

    void StoreInst::print(std::ostream &O) const {
        switch (iType) {
            case Sb:
                O << "sb ";
                break;
            case Sw:
                O << "sw ";
                break;
            case Sd:
                O << "sd ";
                break;
            case Fsw:
                O << "fsw ";
                break;
            case Fsd:
                O << "fsd ";
                break;
            default:
                assert(0 && "Unknown iType");
        }
        Operands[0]->print(O);
        O << ", ";
        Operands[1]->print(O);
        O << std::endl;
    }
} // RISCV