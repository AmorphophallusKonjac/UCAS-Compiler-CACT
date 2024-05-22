#include "EliminateBasicInductionVarPass.h"

#include <utility>
#include "utils/BasicInductionVariable.h"
#include "utils/LoopInfo.h"
#include "IR/iPHINdoe.h"
#include "IR/iOperators.h"

EliminateBasicInductionVarPass::EliminateBasicInductionVarPass(std::string name) : FunctionPass(std::move(name)) {

}

void EliminateBasicInductionVarPass::runOnFunction(IRFunction &F) {
    auto loopList = LoopInfo::findLoop(&F);
    std::vector<IRInstruction *> bin;
    bool codeChanged = true;
    while (codeChanged) {
        codeChanged = false;
        for (auto loop: loopList) {
            auto BISet = BasicInductionVariable::findBasicInductionVar(loop);
            for (auto BI: BISet) {
                auto phi = BI->getPhiNode();
                auto calc = BI->getCalcNode();
                if (phi->getUses().size() == 1 && calc->getUses().size() == 1) {
                    codeChanged = true;
                    bin.push_back(dynamic_cast<IRInstruction *>(phi));
                    bin.push_back(dynamic_cast<IRInstruction *>(calc));
                }
            }
            for (auto trash : bin) {
                auto &instList = trash->getParent()->getInstList();
                instList.erase(std::find(instList.begin(), instList.end(), trash));
            }
            bin.clear();
        }
    }
}
