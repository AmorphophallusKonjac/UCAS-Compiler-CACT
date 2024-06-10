#include "Value.h"
#include "IR/IRValue.h"
#include "BasicBlock.h"
#include "utils/Register.h"
#include "Function.h"

namespace RISCV {
    Register *Value::getReg() const {
        return reg;
    }

    IRValue *Value::getIrValue() const {
        return irValue;
    }

    Value::Value(Register *Reg) : reg(Reg) {
    }

    Value::Value(IRValue *irV) : irValue(irV), reg(irV->getReg()) {
    }

    Value::Value(int val) : val(val) {
    }


    int Value::getVal() const {
        return val;
    }

    void Value::print(std::ostream &O) {
        if (reg) {
            O << reg->getRegName();
        } else if (BB) {
            O << BB->getName();
        } else if (F) {
            O << F->getName();
        } else {
            O << val;
        }
    }

    Value::Value(BasicBlock *BB) : BB(BB) {
    }

    Value::Value(Function *F) : F(F) {
    }

    Pointer::Pointer(int offset) : Value(), offset(offset) {

    }

    void Pointer::print(std::ostream &O) {
        O << offset << "(sp)";
    }
} // RISCV