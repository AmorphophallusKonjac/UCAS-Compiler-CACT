#include "RegisterPass.h"
#include "utils/Register.h"
#include "utils/LiveVariable.h"

RegisterPass::RegisterPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void RegisterPass::runOnFunction(IRFunction &F){
    LiveVariable::genLiveVariable(&F);
    RegisterNode::RegisterAlloc(F, RegisterNode::GENERAL);
    RegisterNode::End();
    RegisterNode::RegisterAlloc(F, RegisterNode::FLOAT);
    RegisterNode::End();
}