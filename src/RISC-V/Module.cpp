#include "Module.h"
#include "GlobalVariable.h"
#include "IR/IRModule.h"

namespace RISCV {
    void Module::addGlobalVariable(GlobalVariable *GV) {
        globalVariableList.push_back(GV);
        globalVariableMap[GV->getIrGv()] = GV;
    }

    Module::Module(IRModule *ir) {
        // add global variable
        for (auto irGV : ir->getVarList()) {
            new GlobalVariable(irGV, this);
        }
    }

    void Module::print(std::ostream &O) {
        for (auto GV : globalVariableList) {
            GV->print(O);
        }
    }
} // RISCV