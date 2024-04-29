#include "Pass.h"
#include "IR/IRModule.h"

void FunctionPass::run(IRModule &M) {
    auto funcList = M.getFuncList();
    for (auto func: funcList) {
        runOnFunction(*func);
    }
}

void BasicBlockPass::runOnFunction(IRFunction &F) {
    auto BBList = F.getBasicBlockList();
    for (auto BB : BBList) {
        runOnBasicBlock(*BB);
    }
}

void Pass::addToOpt(Optimizer &opt) {
    opt.addPass(this);
}

Pass::Pass(std::string name)
    : name(name) {

}

FunctionPass::FunctionPass(std::string name) : Pass(std::move(name)) {

}

BasicBlockPass::BasicBlockPass(std::string name) : FunctionPass(std::move(name)) {

}
