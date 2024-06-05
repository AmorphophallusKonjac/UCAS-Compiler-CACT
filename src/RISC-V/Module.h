#ifndef COMPILER_MODULE_H
#define COMPILER_MODULE_H

#include <vector>
#include <map>
#include <iostream>

class IRGlobalVariable;

class IRModule;

namespace RISCV {

    class GlobalVariable;

    class Function;

    class Module {
    public:
        explicit Module(IRModule *ir);

        void addGlobalVariable(GlobalVariable *GV);

        void addFunction(Function *F);

        void print(std::ostream &O);

        unsigned int getLabelCount();

    private:
        std::vector<GlobalVariable *> globalVariableList;
        std::map<IRGlobalVariable *, GlobalVariable *> globalVariableMap;
        std::vector<Function *> functionList;
        unsigned labelCount;
    };

} // RISCV

#endif //COMPILER_MODULE_H
