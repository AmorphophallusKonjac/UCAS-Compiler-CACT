#include "CutDeadCodePass.h"

#include <utility>

CutDeadCodePass::CutDeadCodePass(std::string name) : BasicBlockPass(std::move(name)) {

}

void CutDeadCodePass::runOnBasicBlock(IRBasicBlock &BB) {
    auto &instList = BB.getInstList();
    std::vector<IRInstruction *> bin;
    for (auto inst: instList) {
        auto uses = inst->getUses();
        auto opcode = inst->getOpcode();
        bool mayWithoutUse = false;
        switch (opcode) {
            case IRInstruction::Ret:
            case IRInstruction::Br:
            case IRInstruction::Store:
            case IRInstruction::Memcpy:
            case IRInstruction::Call:
                mayWithoutUse = true;
                break;
            default:
                mayWithoutUse = false;
                break;
        }
        if (uses.empty() && !mayWithoutUse) {
            bin.push_back(inst);
        }
    }
    for (auto trash: bin) {
        trash->dropAllReferences();
        instList.erase(std::find(instList.begin(), instList.end(), trash));
    }
}
