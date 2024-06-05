#include "Value.h"
#include "IR/IRValue.h"

namespace RISCV {
    Register *Value::getReg() const {
        return reg;
    }

    IRValue *Value::getIrValue() const {
        return irValue;
    }

    Value::Value(Register *Reg) : irValue(nullptr), reg(Reg), val(0), BB(nullptr) {
    }

    Value::Value(IRValue *irV) : irValue(irV), reg(irV->getReg()), val(0), BB(nullptr) {

    }

    Value::Value(int val) : irValue(nullptr), reg(nullptr), val(val), BB(nullptr) {

    }

    int Value::getVal() const {
        return val;
    }

    void Value::print(std::ostream &O) {
        if (reg) {
            O << reg->getRegName();
        } else {
            O << val;
        }
    }

    Value::Value(BasicBlock *BB) : irValue(nullptr), reg(nullptr), val(0), BB(BB) {

    }
} // RISCV