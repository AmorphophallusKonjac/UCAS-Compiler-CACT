#include "IRBasicBlock.h"

#include "IRConstant.h"
#include "IRType.h"
#include "iPHINdoe.h"
#include "iTerminators.h"
#include "IRUse.h"
#include "IRUser.h"
#include <iostream>
void IRBasicBlock::addInstruction(IRInstruction *inst) {
    InstList.push_back(inst);
}
void IRBasicBlock::setParent(IRFunction *parent) {
    this->parent = parent;
}
IRBasicBlock::IRBasicBlock(const std::string &Name, IRFunction *Parent)
    : IRValue(IRType::LabelTy, IRValue::BasicBlockVal, Name) {
    parent = Parent;
    if (Parent)
        parent->addBasicBlock(this);
}
IRBasicBlock::IRBasicBlock(const std::string &Name, IRBasicBlock *InsertBefore)
    : IRValue(IRType::LabelTy, IRValue::BasicBlockVal, Name) {
    parent = InsertBefore->parent;
    InsertBefore->parent->addBasicBlock(this);
}
IRTerminatorInst *IRBasicBlock::getTerminator() {
    return dynamic_cast<IRTerminatorInst *>(InstList.back());
}

void IRBasicBlock::printPrefixName(std::ostream &OS) const{
    OS << "%" << this->getName()  <<std::endl;
}
void IRBasicBlock::print(std::ostream &OS) const {
    //打印每条指令
    this->printPrefixName(OS);
    OS << ":\n" <<std::endl;//这里是否需要做preds，记录可以有哪些块跳转到它？

    /******通过这个uses边去遍历它的user,查明是哪些块使用了它******/
    OS << "                                                ; preds =" <<std::endl;
    for(auto iruseptr: this->getUses()){
        OS << " " << std::endl;//获得使用这个块的终止语句的父块
        dynamic_cast<IRTerminatorInst*>(iruseptr->getUser())->getParent()->printPrefixName(OS);
        OS << "," << std::endl;//获得使用这个块的终止语句的父块
    }

    for(auto inst: this->InstList){
        inst->print(OS);
    }
    // TODO
}
