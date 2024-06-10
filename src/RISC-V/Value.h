#ifndef COMPILER_VALUE_H
#define COMPILER_VALUE_H

#include <iostream>

class Register;

class IRValue;

namespace RISCV {

    class BasicBlock;

    class Function;

    class Value {
    private:
        IRValue *irValue = nullptr;
        Register *reg = nullptr;
        int val = 0;
        BasicBlock *BB = nullptr;
        Function *F = nullptr;
    public:
        Value() = default;

        explicit Value(IRValue *irV);

        explicit Value(Register *Reg);

        explicit Value(int val);

        explicit Value(BasicBlock *BB);

        explicit Value(Function *F);

        Register *getReg() const;

        IRValue *getIrValue() const;

        int getVal() const;

        virtual void print(std::ostream &O);
    };

    class Pointer : public Value {
    private:
        int offset;
    public:
        explicit Pointer(int offset);

        void print(std::ostream &O) override;
    };

} // RISCV

#endif //COMPILER_VALUE_H
