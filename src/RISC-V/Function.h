#ifndef COMPILER_FUNCTION_H
#define COMPILER_FUNCTION_H

#include <string>
#include <vector>
#include <map>
#include "Environment.h"
#include "IR/IRBasicBlock.h"

class IRFunction;

namespace RISCV {

    class Module;

    class BasicBlock;

    class Function {
    public:
        explicit Function(IRFunction *irFunc, Module *parent = nullptr);

        void addBasicBlock(BasicBlock *BB);

        Module *getParent() const;

        void print(std::ostream &O);
    private:
        std::string name;
        Module *parent;
        std::vector<BasicBlock *> BasicBlockList;
        std::map<IRBasicBlock *, BasicBlock *> BasicBlockSet;
        Section section;
        unsigned align;
        BasicBlock *entryBlock;

        void genEntryBlock();

        void generateEntryBlock();

        IRFunction *irFunction;

        unsigned allocSize = 0;

        BasicBlock *findBasicBlock(IRBasicBlock *irBasicBlock);

    public:
        const std::string &getName() const;
    };

} // RISCV

#endif //COMPILER_FUNCTION_H
