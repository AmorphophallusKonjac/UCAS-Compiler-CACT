#include "BasicBlock.h"
#include "IR/IRBasicBlock.h"
#include "Function.h"
#include "Module.h"
#include "Instruction.h"

namespace RISCV {
    void BasicBlock::print(std::ostream &O) {
        if (!name.empty())
            O << name << ":" << std::endl;
        for (auto inst : InstList) {
            O << "\t";
            inst->print(O);
        }
    }

    BasicBlock::BasicBlock(IRBasicBlock *irBasicBlock, Function *parent) : irBasicBlock(irBasicBlock), parent(parent) {
        if (parent) {
            parent->addBasicBlock(this);
        }
    }

    void BasicBlock::addInstruction(Instruction *inst) {
        InstList.push_back(inst);
    }

    IRBasicBlock *BasicBlock::getIrBb() const {
        return irBasicBlock;
    }

    void BasicBlock::generate() {
        // TODO:
    }

    void BasicBlock::setName(const std::string &name) {
        BasicBlock::name = name;
    }
} // RISCV