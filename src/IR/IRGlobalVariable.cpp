#include "IRGlobalVariable.h"
#include "IRConstant.h"
#include <iostream>
void IRGlobalVariable::setParent(IRModule *parent) {
    Parent = parent;
}
void IRGlobalVariable::print(std::ostream &OS) const {
    // TODO
        OS << "Var" << std::endl;
}
IRGlobalVariable::IRGlobalVariable(IRType *Ty, bool isConstant, IRGlobalValue::LinkageTypes Linkage, IRConstant *Initializer, const std::string &Name, IRModule *Parent)
    : IRGlobalValue(Ty, IRValue::GlobalVariableVal, Linkage, Name), isConstantGlobal(isConstant) {
    if (Initializer) {
        Operands.emplace_back(dynamic_cast<IRValue *>(Initializer), this);
    }
    if (Parent) {
        Parent->addGlobalVariable(this);
    }
}
