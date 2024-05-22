#include "Register.h"
#include "utils/LiveVariable.h"

Register::Register(std::string name) : FunctionPass(std::move(name)) {

}

void Register::runOnFunction(IRFunction &F){
    LiveVariable::genLiveVariable(&F);
}