#include "RegisterPass.h"
#include "IR/IRArgument.h"
#include "IR/IRConstant.h"
#include "IR/IRType.h"
#include "IR/IRValue.h"
#include "utils/Register.h"
#include "utils/LiveVariable.h"
#include "utils/RegisterNode.h"
#include <utility>

RegisterPass::RegisterPass(std::string name, int level) : FunctionPass(std::move(name), level) {

}

void setAllInstCalleeConstReg(Register* reg, IRFunction &F){
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            inst->setCalleeSavedLiveReg(reg);
        }
    }
}

void setAllInstCallerConstReg(Register* reg, IRFunction &F){
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            inst->setCallerSavedLiveReg(reg);
        }
    }
}

void RegisterPass::runOnFunction(IRFunction &F){
    LiveVariable::genLiveVariable(&F);
    RegisterNode::RegisterAlloc(F, RegisterNode::GENERAL);
    RegisterNode::End();
    RegisterNode::RegisterAlloc(F, RegisterNode::FLOAT);
    RegisterNode::End();

    std::map<IRConstant*, unsigned> ConstMap;
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

    /*得到排序后的const*/
    std::vector<IRConstant*> FloatConstvec;
    std::vector<IRConstant*> GenConstvec;

    while(!ConstMap.empty()){
        /*每一轮循环找到当前最大的元素*/
        unsigned max=0;
        IRConstant* Const;
        for(auto constreg1: ConstMap){
            if(max < constreg1.second){
                max = constreg1.second;
                Const = constreg1.first;
            }
        }
        /*将当前最大的元素放入vec中，同时从map中erase掉*/
        if( Const->getType()->getPrimitiveID() == IRType::IntTyID ||
            Const->getType()->getPrimitiveID() == IRType::BoolTyID)
            GenConstvec.push_back(Const);
        else if(Const->getType()->getPrimitiveID() == IRType::DoubleTyID ||
                Const->getType()->getPrimitiveID() == IRType::FloatTyID)
            FloatConstvec.push_back(Const);
        ConstMap.erase(Const);
    }

    /*所有剩余类型的寄存器*/
    std::vector<Register*> GPRCallerRegvec;
    std::vector<Register*> GPRCalleeRegvec;
    std::vector<Register*> FPRCallerRegvec;
    std::vector<Register*> FPRCalleeRegvec;

    /*将没有被用到的寄存器分别放到上面的vec中*/
    for(auto Reg: RegisterFactory::getGRegList()){
        auto CalleeList = F.getCalleeSavedRegList();
        auto CallerList = F.getCallerSavedRegList();
        /*发现这个整型Reg在函数中并没有被使用过*/
        if( std::find(CalleeList.begin(), CalleeList.end(), Reg) == CalleeList.end() &&
            std::find(CallerList.begin(), CallerList.end(), Reg) == CallerList.end()){
                switch (Reg->getRegty()) {
                    case Register::CallerSaved:
                    case Register::Param:
                        GPRCallerRegvec.push_back(Reg);
                    case Register::CalleeSaved:
                        GPRCalleeRegvec.push_back(Reg);
                }
            }
    }
    for(auto Reg: RegisterFactory::getFRegList()){
        auto CalleeList = F.getCalleeSavedRegList();
        auto CallerList = F.getCallerSavedRegList();
        /*发现这个浮点型Reg在函数中并没有被使用过*/
        if( std::find(CalleeList.begin(), CalleeList.end(), Reg) == CalleeList.end() &&
            std::find(CallerList.begin(), CallerList.end(), Reg) == CallerList.end()){
                switch (Reg->getRegty()) {
                    case Register::CallerSaved:
                    case Register::Param:
                        FPRCallerRegvec.push_back(Reg);
                    case Register::CalleeSaved:
                        FPRCalleeRegvec.push_back(Reg);
                }
            }
    }

    /*得到reg的数量*/
    unsigned GenRegSize = GPRCallerRegvec.size() + GPRCalleeRegvec.size();
    unsigned FloatRegSize = FPRCallerRegvec.size() + FPRCalleeRegvec.size();

    /*一定要留出一个空闲寄存器(为整型分配)*/
    for(unsigned i=0; i<GenRegSize-1; i++){
        unsigned index;
        if(i<GPRCalleeRegvec.size()){
            index = i;
            GenConstvec[index]->setReg(GPRCalleeRegvec[index]);
            F.setCalleeSavedReg(GPRCalleeRegvec[index]);
            F.setConstRegMap(std::make_pair(GenConstvec[index], GPRCalleeRegvec[index]));
            setAllInstCalleeConstReg(GPRCalleeRegvec[index], F);
        }else{
            index = i-GPRCalleeRegvec.size();
            GenConstvec[index]->setReg(GPRCallerRegvec[index]);
            F.setCallerSavedReg(GPRCallerRegvec[index]);
            F.setConstRegMap(std::make_pair(GenConstvec[index], GPRCallerRegvec[index]));
            setAllInstCallerConstReg(GPRCallerRegvec[index], F);
        }
    }
    /*一定要留出一个空闲寄存器(为浮点型分配)*/
    for(unsigned i=0; i<FloatRegSize-1; i++){
        unsigned index;
        if(i<FPRCalleeRegvec.size()){
            index = i;
            FloatConstvec[index]->setReg(FPRCalleeRegvec[index]);
            F.setCalleeSavedReg(FPRCalleeRegvec[index]);
            F.setConstRegMap(std::make_pair(FloatConstvec[index], FPRCalleeRegvec[index]));
            setAllInstCalleeConstReg(FPRCalleeRegvec[index], F);
        }else{
            index = i-FPRCalleeRegvec.size();
            FloatConstvec[index]->setReg(FPRCallerRegvec[index]);
            F.setCallerSavedReg(FPRCallerRegvec[index]);
            F.setConstRegMap(std::make_pair(FloatConstvec[index], FPRCallerRegvec[index]));
            setAllInstCallerConstReg(FPRCallerRegvec[index], F);
        }
    }

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