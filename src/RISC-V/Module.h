#ifndef COMPILER_MODULE_H
#define COMPILER_MODULE_H

#include <vector>
#include <map>
#include <iostream>

class IRGlobalVariable;

class IRModule;

namespace RISCV {

    class GlobalVariable;

    class Module {
    public:
        explicit Module(IRModule *ir);

        void addGlobalVariable(GlobalVariable *GV);

        void print(std::ostream &O);
    private:
        std::vector<GlobalVariable *> globalVariableList;
        std::map<IRGlobalVariable *, GlobalVariable *> globalVariableMap;

    };

} // RISCV

#endif //COMPILER_MODULE_H
