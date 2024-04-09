#include "IRBasicBlock.h"

#include "IRConstant.h"
#include "IRType.h"
#include "iPHINdoe.h"
#include "iTerminators.h"
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
void IRBasicBlock::print(std::ostream &OS) const {
    // TODO
}
