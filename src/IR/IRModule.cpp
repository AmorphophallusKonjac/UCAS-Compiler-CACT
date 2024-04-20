#include "IRModule.h"
#include <iostream>

#include <utility>
IRModule::IRModule(std::string name) : name(std::move(name)) {
}
void IRModule::addGlobalVariable(IRGlobalVariable *var) {
    varList.push_back(var);
}
void IRModule::addFunction(IRFunction *func) {
    funcList.push_back(func);
}
void IRModule::print(std::ostream &OS) {

    /******打印module_name******/
    OS << "; ModuleID = " << "\'" << this->name << "\'" << std::endl;
    OS << "source_filename = " << "\"" << this->name << "\"" << std::endl;

    /******打印全局变量******/
    for(auto var:this->varList){
        var->print(OS);
    }

    /******打印函数******/
    for(auto func:this->funcList){
        OS << std::endl;
        func->print(OS);
    }
    // TODO
}
IRFunction *IRModule::getFunction(const std::string& name) {
    for (auto func : funcList) {
        if (name == func->getName()) {
            return func;
        }
    }
        return nullptr;
}
IRFunction *IRModule::getMainFunction() {
    return getFunction("main");
}
