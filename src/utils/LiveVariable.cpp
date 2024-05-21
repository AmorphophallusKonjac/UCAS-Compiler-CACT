#include "LiveVariable.h"
#include "IR/IRBasicBlock.h"
#include "IR/IRInstruction.h"


void LiveVariableBB::genLiveVariableBB(IRFunction *F) {
    bool flag=true;
    while(flag){
        for(auto BB:F->getBasicBlockList()){
            BB->getLive()->getOUTLive();
            BB->getLive()->getINLive();
        }
    }
};

void LiveVariableInst::genLiveVariableInst(IRBasicBlock *BB) {

};
