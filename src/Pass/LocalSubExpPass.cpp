#include "LocalSubExpPass.h"
#include "IR/IRValue.h"

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <string>
#include <utility>

void LocalSubExpPass::runOnBasicBlock(IRBasicBlock &BB) {
    bool flagEnd = true;//如果这一轮有子表达式消除，那么就需要再走一轮

    while(flagEnd){
        flagEnd = false;
        for(unsigned i=0; i<BB.getInstList().size(); i++){
            for(unsigned j=i+1; j<BB.getInstList().size();){
                IRInstruction* irinst1 = BB.getInstList()[i];
                IRInstruction* irinst2 = BB.getInstList()[j];

                bool flag = false;
                if(irinst1->getOpcode() == irinst2->getOpcode()){
                    /*如果是可交换的，则两方都需要进行考虑*/
                    if(irinst1->isCommutative()){
                        if( (irinst1->getOperand(0) == irinst2->getOperand(0) && irinst1->getOperand(1) == irinst2->getOperand(1)) ||
                            (irinst1->getOperand(0) == irinst2->getOperand(1) && irinst1->getOperand(1) == irinst2->getOperand(0))){
                                flag = true;
                            }
                    }else{
                        /*按顺序进行检验*/
                        for(unsigned k=0; k<irinst1->getNumOperands(); k++){
                            if(irinst1->getOperand(k) == irinst2->getOperand(k)){ flag = true; }
                            else{ flag = false; break;}
                        }
                    }
                }

                if(flag){//erase完毕后下一条指令自动上前
                    flagEnd = true;
                    auto ircancelinst = std::find(BB.getInstList().begin(), BB.getInstList().end(), irinst2);
                    BB.getInstList().erase(ircancelinst);
                    irinst2->replaceAllUsesWith(irinst1);
                }else{
                    j++;
                }
            }
        }
    }
}

LocalSubExpPass::LocalSubExpPass(std::string name) : BasicBlockPass(name) {

}
