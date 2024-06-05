#ifndef COMPILER_VALUE_H
#define COMPILER_VALUE_H

#include <iostream>

class Register;

class IRValue;

namespace RISCV {

    class BasicBlock;

    class Value {
    private:
        IRValue *irValue;
        Register *reg;
        int val;
        BasicBlock *BB;
    public:
        explicit Value(IRValue *irV);

        explicit Value(Register *Reg);

        explicit Value(int val);

        explicit Value(BasicBlock * BB);

        Register *getReg() const;

        IRValue *getIrValue() const;

        int getVal() const;

        void print(std::ostream &O);
    };

} // RISCV

#endif //COMPILER_VALUE_H
