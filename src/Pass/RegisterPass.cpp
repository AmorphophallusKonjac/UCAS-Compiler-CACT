#include "RegisterPass.h"
#include "IR/IRArgument.h"
#include "IR/IRConstant.h"
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

    std::map<IRConstant*, unsigned>  ConstMap;
    /*遍历每一条指令，得到所有在指令中被使用过的const*/
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            for(unsigned i=0; i < inst->getNumOperands();i++){
                /*如果发现常数，加入map中*/
                if(inst->getOperand(i)->getValueType() == IRValue::ConstantVal){
                    auto constval = dynamic_cast<IRConstant*>(inst->getOperand(i));
                    if(ConstMap.find(constval) != ConstMap.end()){
                        ConstMap[constval]++;
                    }else{
                        ConstMap.insert(std::make_pair(constval, 1));
                    }
                }
            }
        }
    }

    /*每一个inst都根据它的outlive来给出哪些寄存器活跃*/
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            for(auto irlive: *inst->getLive()->getINLive()){
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