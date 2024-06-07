#include "Register.h"
#include "RegisterNode.h"

CalleeSavedRegister::CalleeSavedRegister(unsigned int num) {
    regNum = num;
    if (num <= 1) { regSeq = num + 8; }
    else { regSeq = num + 16; }
    regName = "s" + std::to_string(regNum);
    regty = CalleeSaved;
    regNode = new RegisterNode(regName, this);
}

CallerSavedRegister::CallerSavedRegister(unsigned int num) {
    regNum = num;
    if (num <= 2) { regSeq = num + 5; }
    else { regSeq = num + 25; }
    regName = "t" + std::to_string(regNum);
    regty = CallerSaved;
    regNode = new RegisterNode(regName);
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
