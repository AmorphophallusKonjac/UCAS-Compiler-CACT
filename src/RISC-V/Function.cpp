#include "Function.h"
#include "IR/IRFunction.h"
#include "Module.h"
#include "BasicBlock.h"
#include "BinaryOperator.h"

namespace RISCV {
    Function::Function(IRFunction *irFunc, Module *parent) : section(TEXT), align(1), parent(parent),
                                                             irFunction(irFunc) {
        name = irFunc->getName();
        entryBlock = new BasicBlock(nullptr, nullptr);
        for (auto irBB: irFunc->getBasicBlockList()) {
            new BasicBlock(irBB, this);
        }
        generateEntryBlock();
        for (auto BB: BasicBlockList) {
            BB->generate();
        }
        if (parent)
            parent->addFunction(this);
    }

    void Function::print(std::ostream &O) {
        O << "\t.text" << std::endl;
        O << "\t.align\t" << align << std::endl;
        O << "\t.globl\t" << name << std::endl;
        O << "\t.type\t" << name << ", @function" << std::endl;
        O << name << ":" << std::endl;
        for (auto BB: BasicBlockList) {
            BB->print(O);
        }
        O << "\t.size\t" << name << ", .-" << name << std::endl;
        O << std::endl;
    }

    Module *Function::getParent() const {
        return parent;
    }

    void Function::addBasicBlock(BasicBlock *BB) {
        BasicBlockList.push_back(BB);
        BasicBlockSet[BB->getIrBb()] = BB;
    }

    void Function::generateEntryBlock() {
        entryBlock = new BasicBlock(nullptr, nullptr);
        auto calleeRegList = irFunction->getCalleeSavedRegList();
        unsigned size = 16 * calleeRegList.size();
        for (auto reg: irFunction->getCalleeSavedRegList()) {

        }
    }

    BasicBlock *Function::findBasicBlock(IRBasicBlock *irBasicBlock) {
        return BasicBlockSet[irBasicBlock];
    }

    const std::string &Function::getName() const {
        return name;
    }
} // RISCV