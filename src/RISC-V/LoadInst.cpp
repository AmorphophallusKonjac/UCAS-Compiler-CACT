#include "LoadInst.h"
#include "IR/IRType.h"

namespace RISCV {
    LoadInst::LoadInst(unsigned int iType, Value *rd, Pointer *ptr, BasicBlock *parent)
        : Instruction(IRType::VoidTy, iType, parent) {
        Operands.push_back(rd);
        Operands.push_back(ptr);
    }

    void LoadInst::print(std::ostream &O) const {
        switch (iType) {
            case Lb:
                O << "lb ";
                break;
            case Lw:
                O << "lw ";
                break;
            case Ld:
                O << "ld ";
                break;
            case Flw:
                O << "flw ";
                break;
            case Fld:
                O << "fld ";
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