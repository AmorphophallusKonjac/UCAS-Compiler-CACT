//
// Created by yuanjunkang on 24-5-5.
//

#include "Interpreter.h"
#include "IR/IRConstant.h"

std::vector<TemporaryVariable*> Interpreter::TempVarVector;
std::vector<TemporaryVariable*> Interpreter::GlobalVar;
std::vector<TemporaryVariable*> Interpreter::Stack;

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
        auto allocType = dynamic_cast<IRSequentialType*>(var->getType());
        if(allocType->getPrimitiveID() == IRType::PointerTyID){
            auto tempVarType = getTempVarType(allocType->getElementType());
            auto initialValue = get_initial_value(tempVarType);
            GlobalVar.push_back(new TemporaryVariable(initialValue,tempVarType));
            auto pointer = new TemporaryVariable(GlobalVar.size()-1, TemporaryVariable::Pointer);
            TempVarVector.push_back(pointer);
            var->setTempVar(TempVarVector.back());

            printf("Init Global Variable:");
            std::cout << var->getName() << std::endl;
            GlobalVar.back()->print();
            pointer->print();
            puts("");

        }
        else if(allocType->getPrimitiveID() == IRType::ArrayTyID){
            printf("Error!");
        }
    }
}

TemporaryVariable* Interpreter::interpretFunction(IRFunction *func) {
    TempVarVector.push_back(new TemporaryVariable(0,TemporaryVariable::Func));   // 把函数标识符放入临时变量栈
    Stack.push_back(new TemporaryVariable(0,TemporaryVariable::Func));   // 把函数标识符放入栈
    auto funcArg = func->getArgumentList(); // 获取函数参数
    funcArgPushTempVarVector(funcArg);  // 把函数参数压栈
    auto entryBlock = func -> getEntryBlock();
    auto currentBlock = entryBlock;

InterpretBasicBlock:
    auto instList = currentBlock -> getInstList();

    for(auto inst : instList){
        auto opcode = inst->getOpcode();

        printf("\nInst Name: %s,", inst->getOpcodeName());

        auto operandNum = inst->getNumOperands();

        printf("Operand Number = %d\n", operandNum);
//        for(int i = 0; i < operandNum; ++i){
//            printf("Operand ");
//            std::cout << inst->getOperand(i)->getName() << ": ";
//            change_Operand_To_TemporaryVariable(inst->getOperand(i))->print();
//        }
        switch (opcode) {
            case IRInstruction::Ret : {
                while(TempVarVector.back()->getType() != TemporaryVariable::Func){   // 弹出临时变量，直到遇到函数标识符
                    TempVarVector.pop_back();
                }
                TempVarVector.pop_back();   // 弹出函数标识符

                while(Stack.back()->getType() != TemporaryVariable::Func){   // 弹出局部变量和参数，直到遇到函数标识符
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
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Sub : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 - *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Mul : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 * *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Div : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 / *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Rem : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 % *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::And : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 && *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Or : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 || *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Xor : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 ^ *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetLE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 <= *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetGE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 >= *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetLT : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 < *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetGT : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 > *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetEQ : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 == *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::SetNE : {
                auto tempVar0 = change_Operand_To_TemporaryVariable(inst->getOperand(0));
                auto tempVar1 = change_Operand_To_TemporaryVariable(inst->getOperand(1));
                TemporaryVariable result = *tempVar0 != *tempVar1;
                TempVarVector.push_back(new TemporaryVariable{result.getValue(), result.getType()});
                inst->setTempVar(TempVarVector.back());
                result.print();
                break;
            }

            case IRInstruction::Alloca : {
                auto allocType = dynamic_cast<IRSequentialType*>(inst->getType());
                if(allocType->getPrimitiveID() == IRType::PointerTyID){
                    auto tempVarType = getTempVarType(allocType->getElementType());
                    auto initialValue = get_initial_value(tempVarType);
                    Stack.push_back(new TemporaryVariable(initialValue,tempVarType));
                    auto pointer = new TemporaryVariable(Stack.size()-1, TemporaryVariable::Pointer);
                    TempVarVector.push_back(pointer);
                    inst->setTempVar(TempVarVector.back());
                    Stack.back()->print();
                    pointer->print();
                }
                else if(allocType->getPrimitiveID() == IRType::ArrayTyID){

                }
                break;
            }

            case IRInstruction::Load : {
                auto operand = inst->getOperand(0);
                auto tempVar = change_Operand_To_TemporaryVariable(operand);
                if(tempVar->getType() != TemporaryVariable::Pointer){
                    printf("Error: Load operand is not Pointer type!");
                }
                auto offset = std::any_cast<unsigned long>(tempVar->getValue());
                TemporaryVariable* loadVar;
                if(operand->getValueType() == IRValue::GlobalVariableVal){
                    loadVar = GlobalVar[offset];
                }
                else{
                    loadVar = Stack[offset];
                }
                TempVarVector.push_back(new TemporaryVariable(loadVar->getValue(), loadVar->getType()));
                inst->setTempVar(TempVarVector.back());
                loadVar->print();
                break;
            }

            case IRInstruction::Store : {
                auto operand0 = inst->getOperand(0);
                auto operand1 = inst->getOperand(1);
                auto tempVar0 = change_Operand_To_TemporaryVariable(operand0);
                auto tempVar1 = change_Operand_To_TemporaryVariable(operand1);
                if(tempVar1->getType() != TemporaryVariable::Pointer){
                    printf("Error: Store destination is not Pointer type!");
                }
                auto offset = std::any_cast<unsigned long>(tempVar1->getValue());
                if(operand1->getValueType() == IRValue::GlobalVariableVal){
                    GlobalVar[offset]->setValue(tempVar0->getValue());
                    GlobalVar[offset]->setType(tempVar0->getType());
                    GlobalVar[offset]->print();
                }
                else{
                    Stack[offset]->setValue(tempVar0->getValue());
                    Stack[offset]->setType(tempVar0->getType());
                    Stack[offset]->print();
                }
                break;
            }

            case IRInstruction::Memcpy : {

                break;
            }

            case IRInstruction::PHI : {

                break;
            }

            case IRInstruction::Call : {
                auto dest = inst->getOperand(0);
                dest->getValueType();
                currentBlock = dynamic_cast<IRBasicBlock*>(dest);
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

void Interpreter::funcArgPushTempVarVector(const std::vector<IRArgument *>& argVector) {
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
    auto ty = getTempVarType(irValue->getType());
    switch (ty) {
        case TemporaryVariable::Int : {
            auto constInt = dynamic_cast<IRConstantInt*>(irValue);
            auto val = constInt->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Int};
        }
        case TemporaryVariable::Float : {
            auto constFloat = dynamic_cast<IRConstantFloat*>(irValue);
            auto val = constFloat->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Float};
        }
        case TemporaryVariable::Double : {
            auto constDouble = dynamic_cast<IRConstantDouble*>(irValue);
            auto val = constDouble->getRawValue();
            return new TemporaryVariable{val, TemporaryVariable::Double};
        }
        case TemporaryVariable::Bool : {
            auto constBool = dynamic_cast<IRConstantBool*>(irValue);
            auto val = constBool->getRawValue();
            return new TemporaryVariable{(bool)val, TemporaryVariable::Bool};
        }
        default : {
            printf("Cannot change type:");
            std::cout << TemporaryVariable::getTypeString(ty);
            printf(" to TemporaryVariable\n");
        }
    }
}

std::any Interpreter::get_initial_value(TemporaryVariable::tempVarType ty) {
    switch (ty) {
        case TemporaryVariable::Func :
            return nullptr;
        case TemporaryVariable::Void :
            return nullptr;
        case TemporaryVariable::Int :
            return (int)0;
        case TemporaryVariable::Float :
            return (float)0;
        case TemporaryVariable::Double :
            return (double)0;
        case TemporaryVariable::Bool :
            return false;
        case TemporaryVariable::Pointer :
            return (unsigned long)0;
    }
}
