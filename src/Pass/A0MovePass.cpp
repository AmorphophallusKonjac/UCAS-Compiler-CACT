#include "A0MovePass.h"
#include "IR/iOther.h"

#include <utility>

void A0MovePass::runOnFunction(IRFunction &F) {
    auto BBList = F.getBasicBlockList();
    for (auto BB: BBList) {
        std::vector<IRInstruction *> callInst;
        callInst.clear();
        auto &instList = BB->getInstList();
        for (auto inst: instList) {
            if (IRCallInst::classof(inst) && inst->getType()->getPrimitiveID() != IRType::VoidTyID) {
                callInst.push_back(inst);
            }
        }
        for (auto call: callInst) {
            auto callClone = call->clone();
            callClone->dropAllReferences();
            call->replaceAllUsesWith(callClone);
            auto mv = new IRMoveInst(call, callClone);
            instList.insert(std::find(instList.begin(), instList.end(), call) + 1, mv);
            mv->setParent(BB);
        }
    }
}

A0MovePass::A0MovePass(std::string name, int level) : FunctionPass(std::move(name), level) {

}
