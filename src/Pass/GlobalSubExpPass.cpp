#include "GlobalSubExpPass.h"
#include "IR/IRFunction.h"
#include "IR/IRInstruction.h"
#include "IR/IRValue.h"
#include "Pass/Pass.h"
#include "utils/DominatorTree.h"

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <vector>

std::vector<IRInstruction*> irdomiinstArray;

void childrenSubExp(IRBasicBlock& BB){
        for(auto domi : BB.getNode()->children){
            auto childBB = domi->basicBlock;
            /*用到当前支配路径上的所有可用的指令变量*/
            for(unsigned j=0; j<irdomiinstArray.size();j++){
                for(unsigned i=0; i<childBB->getInstList().size();){
                    IRInstruction* childinst = childBB->getInstList()[i];
                    IRInstruction* parentinst = irdomiinstArray[j];

                    bool flag = false;
                    /*满足两者操作类型相同，并且以下几种指令是不可以消除局部公共子表达式的*/
                    if( (childinst->getOpcode() ==  parentinst->getOpcode()) &&
                        (childinst->getOpcode() != IRInstruction::Br)    &&
                        (childinst->getOpcode() != IRInstruction::PHI)   &&
                        (childinst->getOpcode() != IRInstruction::Load)   &&
                        (childinst->getOpcode() != IRInstruction::Store)   &&
                        (childinst->getOpcode() != IRInstruction::Memcpy)   &&
                        (childinst->getOpcode() != IRInstruction::Call)){
                        /*如果是可交换的，则两方都需要进行考虑*/
                        if(childinst->isCommutative()){
                            if( (childinst->getOperand(0) == parentinst->getOperand(0) && childinst->getOperand(1) == parentinst->getOperand(1)) ||
                                (childinst->getOperand(0) == parentinst->getOperand(1) && childinst->getOperand(1) == parentinst->getOperand(0))){
                                    flag = true;
                                }
                        }else{
                            /*按顺序进行检验*/
                            for(unsigned k=0; k<childinst->getNumOperands(); k++){
                                if(childinst->getOperand(k) == parentinst->getOperand(k)){ flag = true; }
                                else{ flag = false; break;}
                            }
                        }
                    }

                    if(flag){//erase完毕后下一条指令自动上前
                        auto ircancelinst = std::find(childBB->getInstList().begin(), childBB->getInstList().end(), childinst);
                        childinst->dropAllReferences();
                        childBB->getInstList().erase(ircancelinst);
                        childinst->replaceAllUsesWith(parentinst);
                    }else{
                        i++;
                    }
                }
            }

            /*支配路径上的所有指令连同当前块的所有指令放到irchildinstarray里，然后继续沿着支配路径往下走*/
            for(auto inst: childBB->getInstList()){
                irdomiinstArray.push_back(inst);
            }
            childrenSubExp(*childBB);
            for(unsigned k=0; k<childBB->getInstList().size(); k++){
                irdomiinstArray.pop_back();
            }
        }    
}

void GlobalSubExpPass::runOnFunction(IRFunction& F) {
    auto rootBB = F.getEntryBlock();
    irdomiinstArray = rootBB->getInstList();
    childrenSubExp(*rootBB);
}

GlobalSubExpPass::GlobalSubExpPass(std::string name) : FunctionPass(name) {

}
