#ifndef COMPILER_IRMODULE_H
#define COMPILER_IRMODULE_H
#include <string>
#include <vector>

#include "IRFunction.h"
#include "IRGlobalVariable.h"

class IRGlobalVariable;
class FunctionType;

class IRModule {
private:
    std::string name;
    std::vector<IRFunction *> funcList;
    std::vector<IRGlobalVariable *> varList;

public:
    explicit IRModule(std::string name);

    void addGlobalVariable(IRGlobalVariable *var);

    void addFunction(IRFunction *func);

    IRFunction *getFunction(const std::string &name);

    IRFunction *getMainFunction();

    void print();
};


#endif//COMPILER_IRMODULE_H
