#ifndef COMPILER_BINARYOPERATOR_H
#define COMPILER_BINARYOPERATOR_H

#include "Instruction.h"

namespace RISCV {

    class BinaryOperator : public Instruction {
    public:
        BinaryOperator(unsigned iType, Value* rd, Value *op1, Value *op2, BasicBlock *parent = nullptr);

        void print(std::ostream &O) const override;
    };

} // RISCV

#endif //COMPILER_BINARYOPERATOR_H
