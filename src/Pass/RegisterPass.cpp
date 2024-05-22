#include "RegisterPass.h"
#include "utils/LiveVariable.h"

RegisterPass::RegisterPass(std::string name) : FunctionPass(std::move(name)) {

}

void RegisterPass::runOnFunction(IRFunction &F){
    LiveVariable::genLiveVariable(&F);
}