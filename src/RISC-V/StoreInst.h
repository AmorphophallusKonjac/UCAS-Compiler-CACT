
#ifndef COMPILER_STOREINST_H
#define COMPILER_STOREINST_H

#include "Instruction.h"
#include "Value.h"

namespace RISCV {

    class StoreInst : public Instruction {
    public:
        StoreInst(unsigned iType, Value *rd, Pointer *ptr, BasicBlock *parent = nullptr);

        void print(std::ostream &O) const override;
    };

} // RISCV

#endif //COMPILER_STOREINST_H
