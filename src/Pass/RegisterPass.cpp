#include "RegisterPass.h"
#include "IR/IRArgument.h"
#include "IR/IRValue.h"
#include "utils/Register.h"
#include "utils/LiveVariable.h"
#include "utils/RegisterNode.h"

RegisterPass::RegisterPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void RegisterPass::runOnFunction(IRFunction &F){
    LiveVariable::genLiveVariable(&F);
    RegisterNode::RegisterAlloc(F, RegisterNode::GENERAL);
    RegisterNode::End();
    RegisterNode::RegisterAlloc(F, RegisterNode::FLOAT);
    RegisterNode::End();

    /*每一个inst都根据它的outlive来给出哪些寄存器活跃*/
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            for(auto irlive: *inst->getLive()->getOUTLive()){
                if(irlive->getValueType() == IRValue::InstructionVal){
                    switch(dynamic_cast<IRInstruction*>(irlive)->getReg()->getRegty()){
                        case Register::CalleeSaved:
                        case Register::FloatCalleeSaved:
                            inst->setCalleeSavedLiveReg(dynamic_cast<IRInstruction*>(irlive)->getReg());
                            break;
                        case Register::CallerSaved:
                        case Register::FloatCallerSaved:
                        case Register::Param:
                        case Register::FloatParam:
                            inst->setCallerSavedLiveReg(dynamic_cast<IRInstruction*>(irlive)->getReg());
                            break;
                    }
                }else if(irlive->getValueType() == IRValue::ArgumentVal){
                    switch(dynamic_cast<IRArgument*>(irlive)->getReg()->getRegty()){
                        case Register::CalleeSaved:
                        case Register::FloatCalleeSaved:
                            inst->setCalleeSavedLiveReg(dynamic_cast<IRArgument*>(irlive)->getReg());
                            break;
                        case Register::CallerSaved:
                        case Register::FloatCallerSaved:
                        case Register::Param:
                        case Register::FloatParam:
                            inst->setCallerSavedLiveReg(dynamic_cast<IRArgument*>(irlive)->getReg());
                            break;
                    }
                }
            }
        }
    }
}