#include "DeleteNonepredsPass.h"
#include "IR/IRValue.h"
#include "IR/InstrTypes.h"
#include "IR/iPHINdoe.h"
#include <algorithm>

DeleteNonePredsPass::DeleteNonePredsPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void DeleteNonePredsPass::runOnFunction(IRFunction &F) {
    bool flag = true;
    while(flag){
        flag = false;
        for (auto BB: F.getBasicBlockList()) {
            bool isNonePreds = true;
            if(BB->getUses().empty()){
                isNonePreds = true;
            }else{
                /*如果不是空，那么里面只允许有PHI指令*/
                for(auto use: BB->getUses()){
                    if(dynamic_cast<IRInstruction*>(use->getUser())->getOpcode() == IRInstruction::PHI){
                        // /*获得phi指令和phi的位置*/
                        // auto irphi = dynamic_cast<IRPHINode*>(use->getUser());
                        // auto iteirphi = std::find(irphi->getParent()->getInstList().begin(), irphi->getParent()->getInstList().end(), irphi);

                        // /*准备operand进行替代*/
                        // IRValue* replaceinst;
                        // if(irphi->getIncomingBlock(0) == BB)
                        //     replaceinst = irphi->getIncomingValue(0);
                        // if(irphi->getIncomingBlock(1) == BB)
                        //     replaceinst = irphi->getIncomingValue(1);

                        // /*替代掉原先的phi指令*/
                        // irphi->replaceAllUsesWith(replaceinst);
                        // irphi->dropAllReferences();
                        // irphi->getParent()->getInstList().erase(iteirphi);
                        continue;
                    }
                    else{
                        isNonePreds = false;
                        break;
                    }
                }
            }
            /*如果没有人使用这个块，先根据这个块最后一条跳转指令删除掉它的后继，然后把这个块删除*/
            if(isNonePreds && BB != F.getEntryBlock()){
                BB->getTerminator()->dropAllReferences();
                auto iteBB = std::find(F.getBasicBlockList().begin(), F.getBasicBlockList().end(), BB);
                F.getBasicBlockList().erase(iteBB);
                flag = true;
            }
        }
    }
}