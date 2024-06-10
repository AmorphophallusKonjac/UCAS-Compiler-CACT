#ifndef COMPILER_LOADINST_H
#define COMPILER_LOADINST_H

#include "Instruction.h"

#include "Value.h"

namespace RISCV {

    class LoadInst : public Instruction {
    public:
        LoadInst(unsigned iType, Value *rd, Pointer *ptr, BasicBlock *parent = nullptr);

        void print(std::ostream &O) const override;
    };

} // RISCV

#endif //COMPILER_LOADINST_H
