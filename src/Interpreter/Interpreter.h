//
// Created by yuanjunkang on 24-5-5.
//

#ifndef COMPILER_INTERPRETER_H
#define COMPILER_INTERPRETER_H

#include <vector>
#include <any>
#include "IR/IRModule.h"
#include "TemporaryVariable.h"

class Interpreter {
private:
    IRModule *ir;

    static std::vector<TemporaryVariable*> Stack;
    static std::vector<TemporaryVariable*> GlobalVar;
    static TemporaryVariable* interpretFunction(IRFunction *func);
    static void funcArgPushStack(const std::vector<IRArgument *>& argVector);
    static void initGlobalVar(const std::vector<IRGlobalVariable *>& varVector);
    static TemporaryVariable* change_Operand_To_TemporaryVariable(IRValue* irValue);
    static TemporaryVariable* change_ConstantVal_to_TemporaryVariable(IRValue *irValue);
    static TemporaryVariable::tempVarType getTempVarType(IRType* ty);

public:
    explicit Interpreter(IRModule *ir);
    int interpret();

};


#endif //COMPILER_INTERPRETER_H
