#include "Module.h"
#include "GlobalVariable.h"
#include "IR/IRModule.h"
#include "Function.h"

namespace RISCV {
    void Module::addGlobalVariable(GlobalVariable *GV) {
        globalVariableList.push_back(GV);
        globalVariableMap[GV->getIrGv()] = GV;
    }

    Module::Module(IRModule *ir) : labelCount(0) {
        // add global variable
        for (auto irGV: ir->getVarList()) {
            new GlobalVariable(irGV, this);
        }
        // add function
        for (auto irFunc: ir->getFuncList()) {
            if (irFunc->getFuntTy() == IRFunction::UnDeclared)
                continue;
            new Function(irFunc, this);
        }
    }

    void Module::print(std::ostream &O) {
        // print GlobalVariable
        for (auto GV: globalVariableList) {
            GV->print(O);
        }
        // print function
        for (auto func: functionList) {
            func->print(O);
        }
    }

    unsigned int Module::getLabelCount() {
        return labelCount++;
    }

    void Module::addFunction(Function *F) {
        functionList.push_back(F);
    }
} // RISCV