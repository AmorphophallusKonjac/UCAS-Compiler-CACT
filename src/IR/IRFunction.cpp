#include "IRFunction.h"

#include <iostream>

#include "IRModule.h"

void IRFunction::setParent(IRModule *parent) {
    Parent = parent;
}
void IRFunction::print(std::ostream &OS) const {
    // TODO
    OS << "function" << std::endl;
}
IRFunction::IRFunction(IRFunctionType *Ty, IRGlobalValue::LinkageTypes Linkage, const std::string &N, IRModule *M)
    : IRGlobalValue(Ty, IRValue::FunctionVal, Linkage, N) {
    Parent = M;
    for (unsigned i = 0, e = Ty->getNumParams(); i < e; ++i) {
        ArgumentList.push_back(new IRArgument(Ty->getParamType(i)));
    }
    if (M) {
        M->addFunction(this);
    }
}

void IRFunction::addArgument(IRArgument *arg) {
    ArgumentList.push_back(arg);
}
IRFunctionType *IRFunction::getFunctionType() const {
    return dynamic_cast<IRFunctionType *>(getType());
}
const IRType *IRFunction::getReturnType() const {
    return getFunctionType()->getReturnType();
}
void IRFunction::addBasicBlock(IRBasicBlock *block) {
    BasicBlocks.push_back(block);
}
