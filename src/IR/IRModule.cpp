#include "IRModule.h"

#include <utility>
IRModule::IRModule(std::string name) : name(std::move(name)) {
}
void IRModule::addGlobalVariable(IRGlobalVariable *var) {
    varList.push_back(var);
}
void IRModule::addFunction(IRFunction *func) {
    funcList.push_back(func);
}
void IRModule::print() {
    // TODO
}
