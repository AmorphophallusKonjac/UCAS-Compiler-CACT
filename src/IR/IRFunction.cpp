#include "IRFunction.h"

#include <iostream>

#include "IR/IRArgument.h"
#include "IRModule.h"

void IRFunction::setParent(IRModule *parent) {
    Parent = parent;
}
void IRFunction::printPrefixName(std::ostream &OS) const{
    OS << "@" <<this->getName() << " ";
}
void IRFunction::print(std::ostream &OS) const {
    // TODO

    /******打印function开始标识？******/
    OS << "; Function" << std::endl;

    /******打印function本身******/
    OS << "define " ;
    this->getFunctionType()->print(OS);
    this->printPrefixName(OS);

    /******打印arg******/
    OS << "(" ;
    for(auto arg: ArgumentList){
        arg->print(OS);
        OS << ", ";
    }
    OS << ")";

    /******打印basciblock******/
    OS << "{" << std::endl;
    for(auto basicblock:BasicBlocks){
        basicblock->print(OS);
    }
    OS << "}" << std::endl;

}

//这里无法传一个primitiveType的Ty进来，因为在进来之前必须对IRFunctionType给一系列参数进行初始化
IRFunction::IRFunction(IRFunctionType *Ty, IRGlobalValue::LinkageTypes Linkage, const std::string &N, IRModule *M)
    : IRGlobalValue(Ty, IRValue::FunctionVal, Linkage, N) {
    Parent = M;
    /*for (unsigned i = 0, e = args.size(); i < e; ++i) {
        ArgumentList.push_back(new IRArgument(Ty->getParamType(i)));
    }*/
    if (M) {
        M->addFunction(this);
    }
}

void IRFunction::addArgument(IRArgument *arg) {
    ArgumentList.push_back(arg);
}
IRFunctionType *IRFunction::getFunctionType() const {
    return dynamic_cast<IRFunctionType *>(getOriginType());
}
/*const IRType *IRFunction::getReturnType() const {
    return getFunctionType()->getReturnType();
}*/
void IRFunction::addBasicBlock(IRBasicBlock *block) {
    BasicBlocks.push_back(block);
}
