
#include "IR/IRInstruction.h"
#include "IR/iOther.h"
#include "RegisterMove.h"

RegisterMove::RegisterMove(IRMoveInst* moveinst){
    srcNode = dynamic_cast<IRInstruction*>(moveinst->getSrc())->getRegNode();
    dstNode = dynamic_cast<IRInstruction*>(moveinst->getDest())->getRegNode();
}

RegisterMove::RegisterMove(IRArgument* funcarg, Register* reg){
    srcNode = funcarg->getRegNode();
    dstNode = reg->getRegNode();
}