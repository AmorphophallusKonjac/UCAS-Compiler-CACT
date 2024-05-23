#include "LiveVariable.h"
#include "IR/IRBasicBlock.h"
#include "IR/IRConstant.h"
#include "IR/IRInstruction.h"
#include "IR/IRValue.h"
#include "IR/iMemory.h"
#include "IR/iTerminators.h"
#include <algorithm>
#include <vector>

void LiveVariable::genLiveVariable(IRFunction *F) {
    
    /*先生成数据流的活跃变量，然后是块内的活跃变量*/
    LiveVariableBB::genLiveVariableBB(F);
    for(auto BB: F->getBasicBlockList()){
        LiveVariableInst::genLiveVariableInst(BB);
    }
};

void LiveVariableBB::genLiveVariableBB(IRFunction *F) {
    /*IN[BB]=O*/
    for(auto BB: F->getBasicBlockList()){
        BB->getLive()->getINLive().clear();
    }

    bool flag=true;
    while(flag){
        for(auto BB:F->getBasicBlockList()){

            /*OUT[B] = U(S是B的一个后继)IN[S]*/
            for(auto succBB: BB->findSuccessor()){
                for(auto ir: succBB->getLive()->getINLive()){
                    BB->getLive()->getOUTLive().push_back(ir);
                }
            }

            /*use[B] def[B]*/
            std::vector<IRValue*> usevec;
            std::vector<IRValue*> defvec;

            for(auto inst: BB->getInstList()){
                if(inst->isBinaryOp()){ 
                    if(dynamic_cast<IRConstant*>(inst->getOperand(0)))
                        usevec.push_back(inst->getOperand(0));
                    if(dynamic_cast<IRConstant*>(inst->getOperand(1)))
                        usevec.push_back(inst->getOperand(1));
                    defvec.push_back(inst);
                }else if(inst->getOpcode() == IRInstruction::Alloca ||
                         inst->getOpcode() == IRInstruction::Call){
                    defvec.push_back(inst);
                }else if(inst->getOpcode() == IRInstruction::Load ||
                         inst->getOpcode() == IRInstruction::Shl ||
                         inst->getOpcode() == IRInstruction::Shr ){
                    usevec.push_back(inst->getOperand(0));
                    defvec.push_back(inst);
                }else if(inst->getOpcode() == IRInstruction::Store ||
                         inst->getOpcode() == IRInstruction::Memcpy){
                    usevec.push_back(inst->getOperand(0));
                    usevec.push_back(inst->getOperand(1));
                }else if(inst->getOpcode() == IRInstruction::PHI){
                    for(unsigned i=0; i<inst->getNumOperands()/2; i++){
                        usevec.push_back(inst->getOperand(i));

                    }
                    defvec.push_back(inst);
                }else if(inst->getOpcode() == IRInstruction::Ret &&
                         dynamic_cast<IRBranchInst*>(inst)->isConditional()){
                    usevec.push_back(dynamic_cast<IRBranchInst*>(inst)->getCondition());
                }
            }

            std::vector<IRValue*> newINLive;
            auto OUTLive = BB->getLive()->getOUTLive();

            /*OUT[B]*/
            for(auto ir: OUTLive){
                newINLive.push_back(ir);
            }
            /*OUT[B]-def[B]*/
            for(auto ir: defvec){
                auto irout = std::find(newINLive.begin(), newINLive.end(), ir);
                if(irout != defvec.end()){   //找到元素
                    newINLive.erase(irout);
                }
            }
            /*use[B]U(OUT[B]-def[B])*/
            for(auto ir: usevec){
                auto irout = std::find(newINLive.begin(),newINLive.end(), ir);
                if(irout == defvec.end()){   //未找到元素，则加入
                    newINLive.push_back(ir);
                }
            }

            /*IN值是否发生改变*/
            flag = !std::equal(newINLive.begin(), newINLive.end(), BB->getLive()->getINLive().begin(), BB->getLive()->getINLive().end());

            /*IN[B] = use(B)U(OUT[B]-def(B))*/
            BB->getLive()->getINLive() = newINLive;
        }
    }
};

void LiveVariableInst::genLiveVariableInst(IRBasicBlock *BB) {

    for(unsigned i=BB->getInstList().size();i > 0;i--){

        /*OUT[i-1] = IN[i]*/
        if(i == BB->getInstList().size())
            BB->getInstList()[i-1]->getLive()->getOUTLive() = BB->getLive()->getOUTLive();
        else
            BB->getInstList()[i-1]->getLive()->getOUTLive() = BB->getInstList()[i]->getLive()->getINLive();

        auto INLive = BB->getLive()->getINLive();
        auto OUTLive = BB->getLive()->getOUTLive();

        /*use[i-1] def[i-1]*/
        std::vector<IRValue*> usevec;
        std::vector<IRValue*> defvec;
        auto inst= BB->getInstList()[i-1];

        if(inst->isBinaryOp()){ 
            if(dynamic_cast<IRConstant*>(inst->getOperand(0)))
                usevec.push_back(inst->getOperand(0));
            if(dynamic_cast<IRConstant*>(inst->getOperand(1)))
                usevec.push_back(inst->getOperand(1));
            defvec.push_back(inst);
        }else if(inst->getOpcode() == IRInstruction::Alloca ||
                 inst->getOpcode() == IRInstruction::Call){
            defvec.push_back(inst);
        }else if(inst->getOpcode() == IRInstruction::Load ||
                 inst->getOpcode() == IRInstruction::Shl ||
                 inst->getOpcode() == IRInstruction::Shr ){
            usevec.push_back(inst->getOperand(0));
            defvec.push_back(inst);
        }else if(inst->getOpcode() == IRInstruction::Store ||
                 inst->getOpcode() == IRInstruction::Memcpy){
                usevec.push_back(inst->getOperand(0));
                usevec.push_back(inst->getOperand(1));
        }else if(inst->getOpcode() == IRInstruction::PHI){
            for(unsigned i=0; i<inst->getNumOperands()/2; i++){
                usevec.push_back(inst->getOperand(i));
            }
            defvec.push_back(inst);
        }else if(inst->getOpcode() == IRInstruction::Ret &&
                 dynamic_cast<IRBranchInst*>(inst)->isConditional()){
            usevec.push_back(dynamic_cast<IRBranchInst*>(inst)->getCondition());
        }

        /*OUT[i-1]*/
        for(auto ir: OUTLive){
            INLive.push_back(ir);
        }
        /*OUT[i-1]-def[i-1]*/
        for(auto ir: defvec){
            auto irout = std::find(INLive.begin(), INLive.end(), ir);
            if(irout != defvec.end()){   //找到元素
                INLive.erase(irout);
            }
        }
        /*IN[i-1] = use[i-1]U(OUT[i-1]-def[i-1])*/
        for(auto ir: usevec){
            auto irout = std::find(INLive.begin(),INLive.end(), ir);
            if(irout == defvec.end()){   //未找到元素，则加入
                INLive.push_back(ir);
            }
        }
    }
};
