#include "IRGlobalVariable.h"
#include "IRConstant.h"
#include <iostream>
void IRGlobalVariable::setParent(IRModule *parent) {
    Parent = parent;
}

/**
这里调用globalvarible的print，只会在module中初始化全局变量时进行调用打印，原因在于最终在instruction中进行调用打印
的时候规范与这里不尽相同(那个地方只用调用value中name的打印和直接type的打印)
**/

void IRGlobalVariable::print(std::ostream &OS) const {
    // TODO
    /******打印module_name******/
    OS << "@" << this->getName() << std::endl;
    OS << " = global " << std::endl;
    this->getInitializer()->print(OS);//获得初始化use的value值，并进行打印

    /******打印全局变量******/
    for(auto var:this->varList){
        OS << "\n" << std::endl;
        var->print(OS);
    }

    /******打印函数******/
    for(auto func:this->funcList){
        OS << "\n" << std::endl;
        func->print(OS);
    }
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
