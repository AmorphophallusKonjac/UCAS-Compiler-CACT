#include "CutDeadBlockPass.h"
#include "IR/iTerminators.h"

#include <utility>

void CutDeadBlockPass::runOnFunction(IRFunction &F) {
    for (;;) {
        std::vector<IRBasicBlock *> &BBList = F.getBasicBlockList();
        IRBasicBlock *deadBlock;
        deadBlock = nullptr;
        for (auto BB: BBList) {
            auto instList = BB->getInstList();
            auto terminator = BB->getTerminator();
            auto *br = dynamic_cast<IRBranchInst *>(terminator);
            if (instList.size() == 1 && br && br->isUnconditional()) {
                auto newBB = dynamic_cast<IRValue *>(br->getSuccessor(0));
                br->dropAllReferences();
                BB->replaceAllUsesWith(newBB);
                deadBlock = BB;
                break;
            }
        }
        if (deadBlock) {
            BBList.erase(std::find(BBList.begin(), BBList.end(), deadBlock));
        } else {
            break;
        }
    }
}

CutDeadBlockPass::CutDeadBlockPass(std::string name) : FunctionPass(std::move(name)) {

}
