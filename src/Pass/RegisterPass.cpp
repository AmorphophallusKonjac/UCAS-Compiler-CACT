#include "RegisterPass.h"

RegisterPass::RegisterPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void RegisterPass::runOnFunction(IRFunction &F){
    //RegisterNode::RegisterAlloc(F);
}