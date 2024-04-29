#include "RenamePass.h"

#include <utility>

RenamePass::RenamePass(std::string name) : FunctionPass(std::move(name)) {

}

void RenamePass::runOnFunction(IRFunction &F) {
    int cnt = 0;
    auto ArgList = F.getArgumentList();
    auto BBList = F.getBasicBlockList();
    for (auto arg : ArgList) {
        arg->setName(std::to_string(cnt++));
    }
    for (auto BB: BBList) {
        BB->setName(std::to_string(cnt++));
        auto InstList = BB->getInstList();
        for (auto inst : InstList) {
            if (!inst->getName().empty()) {
                inst->setName(std::to_string(cnt++));
            }
        }
    }
}
