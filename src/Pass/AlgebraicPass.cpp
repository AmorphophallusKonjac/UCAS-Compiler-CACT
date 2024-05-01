#include "AlgebraicPass.h"
#include "IR/IRConstant.h"
#include "IR/IRDerivedTypes.h"
#include "IR/IRInstruction.h"
#include "IR/IRValue.h"
#include "IR/InstrTypes.h"
#include "IR/iMemory.h"
#include "IR/iOther.h"
#include "IR/iTerminators.h"

#include <algorithm>
#include <string>
#include <utility>

void AlgebraicPass::mul2shift(IRInstruction* irinst, IRBasicBlock &BB){
    std::vector<IRInstruction*> irinstArray;
    IRValue* irvalue;
    IRConstantInt* irconst;

    /*!
     * 根据传入的inst判断const和irvalue
     */
    if(dynamic_cast<IRConstantInt*>(irinst->getOperand(0))){
        irconst = dynamic_cast<IRConstantInt*>(irinst->getOperand(0));
        irvalue = dynamic_cast<IRConstantInt*>(irinst->getOperand(1));
    }else if(dynamic_cast<IRConstantInt*>(irinst->getOperand(1))){
        irconst = dynamic_cast<IRConstantInt*>(irinst->getOperand(1));
        irvalue = dynamic_cast<IRConstantInt*>(irinst->getOperand(0));
    }else{
        return;
    }

    int num = irconst->getRawValue();
    int index = 0;
 
    /*消除inst*/
    auto mulinst = std::find(BB.getInstList().begin(), BB.getInstList().end(), irinst);
    BB.getInstList().erase(mulinst);

    /*移位inst*/
    while(num != 0){
        if(index%2 == 1){
            irinstArray.push_back(
                new ShiftInst(IRInstruction::Shl, irvalue, IRConstantInt::get(index), std::to_string(BB.getParent()->getCount())));
            BB.getParent()->addCount();
        }
        num>>1;
        index++;
    }

    /*store inst*/
    for(unsigned i=0; i<irinstArray.size()-1; i++){
        irinstArray.push_back(
            IRBinaryOperator::create(IRInstruction::Add, irinstArray[i], irinstArray[i+1], std::to_string(BB.getParent()->getCount())));
    }
    
    /*全部替代*/
    irinst->replaceAllUsesWith(irinstArray.back());
}

void AlgebraicPass::runOnBasicBlock(IRBasicBlock &BB) {
    for(auto inst: BB.getInstList()){
        if(inst->getOpcode() == IRInstruction::Mul){
            mul2shift(inst, BB);
        }
    }
}

AlgebraicPass::AlgebraicPass(std::string name) : BasicBlockPass(name) {

}
