#ifndef COMPILER_USER_H
#define COMPILER_USER_H

#include "IR/IRValue.h"
#include <vector>
#include <cassert>

namespace RISCV {

    class User {
    protected:
        std::vector<IRValue *> Operands;
    public:
        IRValue *getOperand(unsigned i) {
            assert(i < Operands.size() && "getOperand out of range!");
            return Operands[i];
        }

        inline unsigned getNumOperands() const { return Operands.size(); }

        void dropAllReferences() {
            Operands.clear();
        }
    };

} // RISCV

#endif //COMPILER_USER_H
