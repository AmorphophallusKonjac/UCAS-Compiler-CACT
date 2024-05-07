//
// Created by yuanjunkang on 24-5-5.
//

#include "Interpreter.h"
#include "IR/IRConstant.h"

std::vector<TemporaryVariable*> Interpreter::Stack;
std::vector<TemporaryVariable*> Interpreter::GlobalVar;

Interpreter::Interpreter(IRModule *ir) : ir(ir) {
}

int Interpreter::interpret() {
    auto mainFunc = ir -> getMainFunction();
    auto varList = ir -> getVarList();
    initGlobalVar(varList);
    TemporaryVariable* ret = interpretFunction(mainFunc);
    return std::any_cast<int>(ret->getValue());
}


void Interpreter::initGlobalVar(const std::vector<IRGlobalVariable *>& varVector) {
    auto varVectorSize = varVector.size();
    for(auto var : varVector){
        auto varValue = var->getOperand(0);
        TemporaryVariable* globalVar = change_ConstantVal_to_TemporaryVariable(varValue);
        printf("Init Global Variable: "); globalVar->print();
        GlobalVar.push_back(globalVar);
        auto addr = &GlobalVar.back();
        var->setTempVar(GlobalVar.back());
    }
}

TemporaryVariable* Interpreter::interpretFunction(IRFunction *func) {
    TemporaryVariable func_identifier = TemporaryVariable(0,TemporaryVariable::tempVarType::Func);
    Stack.push_back(new TemporaryVariable{func_identifier.getValue(), func_identifier.getType()});   // 把函数标识符压栈
    auto funcArg = func->getArgumentList();
    funcArgPushStack(funcArg);  // 把参数压栈
    auto entryBlock = func -> getEntryBlock();
    auto currentBlock = entryBlock;

InterpretBasicBlock:
    auto instList = currentBlock -> getInstList();

    for(auto inst : instList){
        auto opcode = inst->getOpcode();

        printf("Inst Name: %s,", inst->getOpcodeName());

        auto operandNum = inst->getNumOperands();

        printf("Operand Number = %d\n", operandNum);
        for(int i = 0; i < operandNum; ++i){
            printf("Operand %d: ", i);
            change_Operand_To_TemporaryVariable(inst->getOperand(i))->print();
        }
        switch (opcode) {
            case IRInstruction::Ret : {
                while(Stack.back()->getType() != TemporaryVariable::Func){   // 出栈，直到遇到函数标识符
                    Stack.pop_back();
                }
                Stack.pop_back();   // 弹出函数标识符
                auto operand = inst->getOperand(0);
                return change_Operand_To_TemporaryVariable(operand);
            }
            case IRInstruction::Br : {
                if(operandNum == 1) {
                    auto dest = inst->getOperand(0);
                    dest->getValueType();
                    currentBlock = dynamic_cast<IRBasicBlock*>(dest);
                }
                else{
                    if(operandNum != 3){
                        printf("Undefined Branch Inst With Operand Number = %d\n");
                    }

                    auto destTrue = inst->getOperand(0);
                    auto destFalse = inst->getOperand(1);
                    auto cond = inst->getOperand(2);
                    auto tempVarCond = change_Operand_To_TemporaryVariable(cond);

                    if(tempVarCond->getType() != TemporaryVariable::Bool){
                        printf("Cannot Branch With Condition Type: ");
                        std::cout << TemporaryVariable::getTypeString(tempVarCond->getType()) << std::endl;
                    }

                    if(std::any_cast<bool>(tempVarCond->getValue())){
                        currentBlock = dynamic_cast<IRBasicBlock*>(destTrue);
                    }
                    else{
                        currentBlock = dynamic_cast<IRBasicBlock*>(destFalse);
                    }
                }
                goto InterpretBasicBlock;
            }

            case IRInstruction::Add : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 + *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Sub : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 - *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Mul : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 * *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Div : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 / *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Rem : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 % *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::And : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 && *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Or : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 || *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Xor : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 ^ *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetLE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 <= *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetGE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 >= *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetLT : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 < *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetGT : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 > *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetEQ : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 == *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::SetNE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 != *tempVar1;
                Stack.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(Stack.back());
                break;
            }

            case IRInstruction::Alloca : {
                auto tempVar = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                Stack.push_back(tempVar);
                inst->setTempVar(Stack.back());
                tempVar->print();
                break;
            }

            case IRInstruction::Load : {
                auto tempVar = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                Stack.push_back(tempVar);
                inst->setTempVar(Stack.back());
                tempVar->print();
                break;
            }

            case IRInstruction::Store : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                tempVar1->setValue(tempVar0->getValue());
                tempVar1->setType(tempVar0->getType());
                tempVar1->print();
                break;
            }

            case IRInstruction::Memcpy : {

                break;
            }

            case IRInstruction::PHI : {

                break;
            }

            case IRInstruction::Call : {

                break;
            }

            case IRInstruction::Shl : {

                break;
            }

            case IRInstruction::Shr : {

                break;
            }

            default:{
                printf("Undefined Operator: %s\n", inst->getOpcodeName());
            }
        }
    }
}

void Interpreter::funcArgPushStack(const std::vector<IRArgument *>& argVector) {
    for(auto arg : argVector){

    }
}

TemporaryVariable* Interpreter::change_Operand_To_TemporaryVariable(IRValue *irValue) {
    auto valueType = irValue -> getValueType();
    if (valueType == IRValue::ArgumentVal || valueType == IRValue::InstructionVal
        || valueType == IRValue::GlobalVariableVal) {
        return irValue->getTempVar();
    }
    else if(valueType == IRValue::ConstantVal) {
        return change_ConstantVal_to_TemporaryVariable(irValue);
    }
    else {
        printf("Cannot change IRValue: ");
        std::cout << irValue->getName() << std::endl;
        printf(" to TemporaryVariable");
    }
}

TemporaryVariable::tempVarType Interpreter::getTempVarType(IRType* ty){
    switch(ty->getPrimitiveID()){
        case IRType::VoidTyID : {
            return TemporaryVariable::tempVarType::Void;
        }
        case IRType::BoolTyID : {
            return TemporaryVariable::tempVarType::Bool;
        }
        case IRType::IntTyID : {
            return TemporaryVariable::tempVarType::Int;
        }
        case IRType::FloatTyID : {
            return TemporaryVariable::tempVarType::Float;
        }
        case IRType::DoubleTyID : {
            return TemporaryVariable::tempVarType::Double;
        }
        case IRType::PointerTyID : {
            return TemporaryVariable::tempVarType::Pointer;
        }
        default: {
            printf("Cannot change IRType to tempVarType");
        }
    }
}

TemporaryVariable* Interpreter::change_ConstantVal_to_TemporaryVariable(IRValue *irValue) {
    auto constValue = dynamic_cast<IRConstant *>(irValue);
    auto ty = getTempVarType(irValue->getType());
    switch (ty) {
        case TemporaryVariable::Int : {
            auto constInt = dynamic_cast<IRConstantInt*>(irValue);
            auto val = constInt->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Int};
        }
        case TemporaryVariable::Float : {
            auto constInt = dynamic_cast<IRConstantFloat*>(irValue);
            auto val = constInt->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Float};
        }
        case TemporaryVariable::Double : {
            auto constInt = dynamic_cast<IRConstantDouble*>(irValue);
            auto val = constInt->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Double};
        }
        case TemporaryVariable::Bool : {
            auto constInt = dynamic_cast<IRConstantBool*>(irValue);
            auto val = constInt->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Bool};
        }
        default : {
            printf("Cannot change type:");
            std::cout << TemporaryVariable::getTypeString(ty);
            printf(" to TemporaryVariable\n");
        }
    }
}
