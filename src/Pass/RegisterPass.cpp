#include "RegisterPass.h"
#include "utils/Register.h"

RegisterPass::RegisterPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void RegisterPass::runOnFunction(IRFunction &F){
    RegisterNode::RegisterAlloc(F, RegisterNode::GENERAL);
    //RegisterNode::RegisterAlloc(F, RegisterNode::FLOAT);
}