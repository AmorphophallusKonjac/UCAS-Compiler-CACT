#include "PHIdeletePass.h"
#include "IR/IRInstruction.h"
#include "IR/iOther.h"
#include "IR/iPHINdoe.h"
#include "utils/LiveVariable.h"
#include <algorithm>

PHIdeletePass::PHIdeletePass(std::string name) : FunctionPass(std::move(name)) {

}

void PHIdeletePass::runOnFunction(IRFunction &F) {
    for (auto BB: F.getBasicBlockList()) {
        for (auto inst: BB->getInstList()) {
            if (inst->getOpcode() == IRInstruction::PHI) {
                auto phiinst = dynamic_cast<IRPHINode *>(inst);
                for (unsigned i = 0; i < phiinst->getNumIncomingValues(); i++) {
                    /*add move inst*/
                    auto mvInst = new IRMoveInst(phiinst->getIncomingValue(i), inst);
                    mvInst->setParent(phiinst->getIncomingBlock(i));
                    phiinst->getIncomingBlock(i)->getInstList().insert(
                            phiinst->getIncomingBlock(i)->getInstList().end() - 1,
                            mvInst);
                }
                /*delete phi inst*/
                auto irinst = std::find(inst->getParent()->getInstList().begin(),
                                        inst->getParent()->getInstList().end(), inst);
                inst->getParent()->getInstList().erase(irinst);
            }
        }
    }
}