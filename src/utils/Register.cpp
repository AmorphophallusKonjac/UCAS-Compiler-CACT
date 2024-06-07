#include "Register.h"
#include "RegisterNode.h"
#include "IR/IRFunction.h"
#include "IR/IRBasicBlock.h"
#include "IR/iOther.h"
#include <ostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <utility>

std::vector<Register *> RegisterFactory::GeneralRegList = {};
std::vector<Register *> RegisterFactory::FloatRegList = {};
std::vector<CallerSavedRegister *> CallerSavedRegister::CallerSavedvec = {};
std::vector<CalleeSavedRegister *> CalleeSavedRegister::CalleeSavedvec = {};
std::vector<ParamRegister *> ParamRegister::Paramvec = {};
std::vector<FloatCallerSavedRegister *> FloatCallerSavedRegister::FloatCallerSavedvec = {};
std::vector<FloatCalleeSavedRegister *> FloatCalleeSavedRegister::FloatCalleeSavedvec = {};
std::vector<FloatParamRegister *> FloatParamRegister::FloatParamvec = {};

CallerSavedRegister::CallerSavedRegister(unsigned int num) {
    regNum = num;
    if (num <= 2) { regSeq = num + 5; }
    else { regSeq = num + 25; }
    regName = "t" + std::to_string(regNum);
    regty = CallerSaved;
    regNode = new RegisterNode(regName, this);
}

CalleeSavedRegister::CalleeSavedRegister(unsigned int num) {
    regNum = num;
    if (num <= 1) { regSeq = num + 8; }
    else { regSeq = num + 16; }
    regName = "s" + std::to_string(regNum);
    regty = CalleeSaved;
    regNode = new RegisterNode(regName, this);
}

ParamRegister::ParamRegister(unsigned int num) {
    regNum = num;
    regSeq = num + 10;
    regName = "a" + std::to_string(regNum);
    regty = Param;
    regNode = new RegisterNode(regName, this);
}

FloatCallerSavedRegister::FloatCallerSavedRegister(unsigned int num) {
    regNum = num;
    regSeq = num + 16;
    regName = "f" + std::to_string(regSeq);
    regty = FloatCallerSaved;
    regNode = new RegisterNode(regName, this);
}

FloatCalleeSavedRegister::FloatCalleeSavedRegister(unsigned int num) {
    regNum = num;
    regSeq = num + 8;
    regName = "f" + std::to_string(regSeq);
    regty = FloatCalleeSaved;
    regNode = new RegisterNode(regName, this);
}

FloatParamRegister::FloatParamRegister(unsigned int num) {
    regNum = num;
    regSeq = num;
    regName = "f" + std::to_string(regSeq);
    regty = FloatParam;
    regNode = new RegisterNode(regName, this);
}

void RegisterFactory::print(std::ostream& OS, IRFunction& F){
    for(auto reg: getGRegList()){
        OS << reg->getRegName() << ": ";
        for(auto BB: F.getBasicBlockList()){
            for(auto inst: BB->getInstList()){
                if(inst->getOpcode() == IRInstruction::Move){
                    if(dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(inst)->getDest())->getReg() == reg){
                        OS << dynamic_cast<IRMoveInst*>(inst)->getDest()->getName() << ", ";
                    }
                }else{
                    if(inst->getReg() == reg){
                        OS << inst->getName() << ", ";
                    }
                }
            }
        }
        OS << std::endl;
    }
    for(auto reg: getFRegList()){
        OS << reg->getRegName() << ": ";
        for(auto BB: F.getBasicBlockList()){
            for(auto inst: BB->getInstList()){
                if(inst->getOpcode() == IRInstruction::Move){
                    if(dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(inst)->getDest())->getReg() == reg){
                        OS << dynamic_cast<IRMoveInst*>(inst)->getDest()->getName() << ", ";
                    }
                }else{
                    if(inst->getReg() == reg){
                        OS << inst->getName() << ", ";
                    }
                }
            }
        }
        OS << std::endl;
    }
}

void RegisterFactory::initReg() {
    CallerSavedRegister::initTreg();
    CalleeSavedRegister::initSreg();
    ParamRegister::initAreg();
    FloatCallerSavedRegister::initFTreg();
    FloatCalleeSavedRegister::initFSreg();
    FloatParamRegister::initFAreg();

    GeneralRegList.reserve(CallerSavedRegister::getTregList().size() +
                              CalleeSavedRegister::getSregList().size() +
                              ParamRegister::getAregList().size());
    std::copy(CallerSavedRegister::getTregList().begin(), CallerSavedRegister::getTregList().end(), std::back_inserter(GeneralRegList));
    std::copy(CalleeSavedRegister::getSregList().begin(), CalleeSavedRegister::getSregList().end(), std::back_inserter(GeneralRegList));
    std::copy(ParamRegister::getAregList().begin(), ParamRegister::getAregList().end(), std::back_inserter(GeneralRegList));

    FloatRegList.reserve(FloatCallerSavedRegister::getFTregList().size() +
                              FloatCalleeSavedRegister::getFSregList().size() +
                              FloatParamRegister::getFAregList().size());
    std::copy(FloatCallerSavedRegister::getFTregList().begin(), FloatCallerSavedRegister::getFTregList().end(), std::back_inserter(FloatRegList));
    std::copy(FloatCalleeSavedRegister::getFSregList().begin(), FloatCalleeSavedRegister::getFSregList().end(), std::back_inserter(FloatRegList));
    std::copy(FloatParamRegister::getFAregList().begin(), FloatParamRegister::getFAregList().end(), std::back_inserter(FloatRegList));

}
