#include "ConstantPass.h"
#include "IR/IRConstant.h"
#include "IR/IRInstruction.h"
#include "IR/IRType.h"
#include "IR/IRValue.h"
#include "IR/iTerminators.h"
#include "Pass/Pass.h"
#include <typeinfo>

#include <any>
#include <utility>

/*非bool类型*/
std::any alge(unsigned iType, IRType::PrimitiveID tyID, std::any const1, std::any const2){
    switch(iType){
        case IRInstruction::BinaryOps::Add:
            if(tyID == IRType::IntTyID){ return std::any_cast<int>(const1)+std::any_cast<int>(const2); }
            else if(tyID == IRType::FloatTyID){ return std::any_cast<float>(const1)+std::any_cast<float>(const2); }
            else if(tyID == IRType::DoubleTyID){ return std::any_cast<double>(const1)+std::any_cast<double>(const2); }
        case IRInstruction::BinaryOps::Sub:
            if(tyID == IRType::IntTyID){ return std::any_cast<int>(const1)-std::any_cast<int>(const2); }
            else if(tyID == IRType::FloatTyID){ return std::any_cast<float>(const1)-std::any_cast<float>(const2); }
            else if(tyID == IRType::DoubleTyID){ return std::any_cast<double>(const1)-std::any_cast<double>(const2); }
        case IRInstruction::BinaryOps::Mul:
            if(tyID == IRType::IntTyID){ return std::any_cast<int>(const1)*std::any_cast<int>(const2); }
            else if(tyID == IRType::FloatTyID){ return std::any_cast<float>(const1)*std::any_cast<float>(const2); }
            else if(tyID == IRType::DoubleTyID){ return std::any_cast<double>(const1)*std::any_cast<double>(const2); }
        case IRInstruction::BinaryOps::Div:
            if(tyID == IRType::IntTyID){ return std::any_cast<int>(const1)/std::any_cast<int>(const2); }
            else if(tyID == IRType::FloatTyID){ return std::any_cast<float>(const1)/std::any_cast<float>(const2); }
            else if(tyID == IRType::DoubleTyID){ return std::any_cast<double>(const1)/std::any_cast<double>(const2); }
        case IRInstruction::BinaryOps::Rem:
            if(tyID == IRType::IntTyID){ return std::any_cast<int>(const1)%std::any_cast<int>(const2); }
    }
}

/*bool类型*/
bool judge(unsigned iType, std::any const1, std::any const2){
    switch(iType){
        case IRInstruction::BinaryOps::Xor:
            return std::any_cast<bool>(const1)^std::any_cast<bool>(const2);
        case IRInstruction::BinaryOps::Or:
            return std::any_cast<bool>(const1)||std::any_cast<bool>(const2);
        case IRInstruction::BinaryOps::And:
            return std::any_cast<bool>(const1)&&std::any_cast<bool>(const2); 
    }
}

void ConstantPass::runOnBasicBlock(IRBasicBlock &BB) {
    IRConstant* irnewconst;
    for (std::vector<IRInstruction*>::iterator instIterator= BB.getInstList().begin(); instIterator != BB.getInstList().end();) {
        auto inst = *instIterator;
        if( IRInstruction::BinaryOps::Add <= inst->getOpcode()  &&
            inst->getOpcode() <= IRInstruction::BinaryOps::Xor  && 
            inst->getOperand(0)->getValueType() == IRInstruction::ConstantVal   &&
            inst->getOperand(1)->getValueType() == IRInstruction::ConstantVal){
                /*消除该条指令，并且用新得到的irnewconst代替所有的uses*/
                if(dynamic_cast<IRConstantBool*>(inst->getOperand(0))){
                    irnewconst = IRConstantBool::get
                    (judge(inst->getOpcode(), 
                    dynamic_cast<IRConstantBool*>(inst->getOperand(0))->getRawValue(), 
                    dynamic_cast<IRConstantBool*>(inst->getOperand(1))->getRawValue()));
                }else if(dynamic_cast<IRConstantInt*>(inst->getOperand(0))){
                    irnewconst = IRConstantInt::get
                    (std::any_cast<int>(alge(inst->getOpcode(), IRType::IntTyID,
                                            dynamic_cast<IRConstantInt*>(inst->getOperand(0))->getRawValue(), 
                                            dynamic_cast<IRConstantInt*>(inst->getOperand(1))->getRawValue())));
                }else if(dynamic_cast<IRConstantFloat*>(inst->getOperand(0))){
                    irnewconst = IRConstantFloat::get
                    (std::any_cast<float>(alge(inst->getOpcode(), IRType::FloatTyID,
                                            dynamic_cast<IRConstantFloat*>(inst->getOperand(0))->getRawValue(), 
                                            dynamic_cast<IRConstantFloat*>(inst->getOperand(1))->getRawValue())));
                }else if(dynamic_cast<IRConstantDouble*>(inst->getOperand(0))){
                    irnewconst = IRConstantDouble::get
                    (std::any_cast<double>(alge(inst->getOpcode(), IRType::DoubleTyID,
                                            dynamic_cast<IRConstantDouble*>(inst->getOperand(0))->getRawValue(), 
                                            dynamic_cast<IRConstantDouble*>(inst->getOperand(1))->getRawValue())));
                }

                instIterator = BB.getInstList().erase(instIterator);
                inst->replaceAllUsesWith(irnewconst);

            }else{
                instIterator++;
            }
    }
}

ConstantPass::ConstantPass(std::string name) : BasicBlockPass(std::move(name)) {

}
