#include "IRFunction.h"

#include <iostream>

#include "IRModule.h"

void IRFunction::setParent(IRModule *parent) {
    Parent = parent;
}
void IRFunction::printPrefixName(std::ostream &OS) const{
    OS << "@" <<this->getName() << std::endl;
}
void IRFunction::print(std::ostream &OS) const {
    // TODO

    /******打印function开始标识？******/
    OS << "; Function\n" << std::endl;

    /******打印function本身******/
    OS << "define " << std::endl;
    this->getFunctionType()->print(OS);
    this->printPrefixName(OS);

    /******打印arg******/
    OS << "(" << std::endl;
    for(auto arg: ArgumentList){
        arg->print(OS);
    }
    OS << ")" << std::endl;

    /******打印basciblock******/
    OS << "{\n" << std::endl;
    for(auto basicblock:BasicBlocks){
        basicblock->print(OS);
    }
    OS << "}" << std::endl;

}

//这里无法传一个primitiveType的Ty进来，因为在进来之前必须对IRFunctionType给一系列参数进行初始化
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
/*const IRType *IRFunction::getReturnType() const {
    return getFunctionType()->getReturnType();
}*/
void IRFunction::addBasicBlock(IRBasicBlock *block) {
    BasicBlocks.push_back(block);
}
