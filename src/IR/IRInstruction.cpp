#include "IRInstruction.h"

#include <iostream>
#include <ostream>

#include "IR/IRBasicBlock.h"
#include "IR/IRDerivedTypes.h"
#include "IR/IRValue.h"
#include "IRConstant.h"
#include "IRType.h"
#include "IRValue.h"
#include "iMemory.h"
#include "iOther.h"
#include "iPHINdoe.h"
#include "iTerminators.h"
IRInstruction::IRInstruction(IRType *Ty, unsigned int iType, const std::string &Name, IRBasicBlock *parent)
    : IRUser(Ty, InstructionVal, Name) {
    Parent = parent;
    this->iType = iType;

    if (parent) {
        parent->addInstruction(this);
    }
}

void IRInstruction::setParent(IRBasicBlock *P) {
    Parent = P;
}

const char *IRInstruction::getOpcodeName(unsigned int OpCode) {
    switch (OpCode) {
        // Terminators
        case Ret:
            return "ret";
        case Br:
            return "br";

        // Standard binary operators...
        case Add:
            return "add";
        case Sub:
            return "sub";
        case Mul:
            return "mul";
        case Div:
            return "div";
        case Rem:
            return "rem";

        // Logical operators...
        case And:
            return "and";
        case Or:
            return "or";
        case Xor:
            return "xor";

        // SetCC operators...
        case SetLE:
            return "setle";
        case SetGE:
            return "setge";
        case SetLT:
            return "setlt";
        case SetGT:
            return "setgt";
        case SetEQ:
            return "seteq";
        case SetNE:
            return "setne";

        // Memory instructions...
        case Alloca:
            return "alloca";
        case Load:
            return "load";
        case Store:
            return "store";
        case Memcpy:
            return "memcpy";

        // Other instructions...
        case PHI:
            return "phi";
        case Call:
            return "call";
        case Shl:
            return "shl";
        case Shr:
            return "shr";

        default:
            return "<Invalid operator> ";
    }
}

bool IRInstruction::isAssociative(unsigned int op, const IRType *Ty) {
    if (op == Add || op == Mul ||
        op == And || op == Or || op == Xor) {
        // Floating point operations do not associate!
        return !Ty->isFloatingPoint();
    }
    return false;
}

bool IRInstruction::isCommutative(unsigned int op) {
    switch (op) {
        case Add:
        case Mul:
        case And:
        case Or:
        case Xor:
        case SetEQ:
        case SetNE:
            return true;
        default:
            return false;
    }
}
/**
这里一直有一个严重的问题想要说明一下：对于一个全局变量而言，它的print方法实际上是对于它的声明时来进行使用的，basicBlock同理;
因此在这里进行打印的时候，我对于这两个类一集其他相似情况的类我从来都不会调用它们的print方法而是单独打印
相对应的IRconstant是一个例外，因为我从来都不需要对它进行声明
**/
void IRInstruction::BinaryLogicalIRInstPrint(std::ostream &OS, bool AskFloat) const {
    this->printPrefixName(OS);//打印instructionName
    OS << " = " << std::endl;
    if (this->getType()->isFloatingPoint() && AskFloat) {
        OS << "f" << std::endl;//打印f
    }
    OS << this->getOpcodeName() << " " << std::endl;//打印add
    this->getType()->print(OS);                     //打印type
    this->getOperand(0)->printPrefixName(OS);       //打印第一个操作数
    this->getOperand(1)->printPrefixName(OS);       //打印第二个操作数
}

void IRInstruction::SetCCIRInstPrint(std::ostream &OS) const {
    this->printPrefixName(OS);//打印instructionName
    OS << " = " << std::endl;
    if (this->getType()->isFloatingPoint()) {
        OS << "fcmp " << std::endl;//打印fcmp
    } else {
        OS << "icmp " << std::endl;//打印icmp
    }
    OS << this->getOpcodeName() << " " << std::endl;//打印set
    this->getType()->print(OS);                     //打印type
    this->getOperand(0)->printPrefixName(OS);       //打印第一个操作数
    this->getOperand(1)->printPrefixName(OS);       //打印第二个操作数
}
void IRInstruction::printPrefixName(std::ostream &OS) const {
    OS << "%" << this->getName();
}
void IRInstruction::print(std::ostream &OS) const {

    IRValue *operand;
    IRValue *operand1;
    IRValue *operand2;

    switch (getOpcode()) {
        // Terminators
        case Ret:
            //instruction begin
            OS << this->getOpcodeName() << " " << std::endl;//打印ret

            //打印返回值
            this->Operands[0].get()->print(OS);//打印一个IRconstant
            break;
        case Br:
            //instruction begin
            OS << this->getOpcodeName() << " " << std::endl;//打印ret

            /******根据irbranch是不是有条件跳转来进行相对应的具体标号打印******/
            const IRBranchInst *irbranch;
            irbranch = dynamic_cast<const IRBranchInst *>(this);
            if (irbranch->isConditional()) {//有条件判断
                //打印一个IRvalue
                irbranch->Operands[2].get()->print(OS);
                //打印两个label跳转标号
                OS << ", " << std::endl;
                OS << "label " << irbranch->Operands[0].get()->getName() << std::endl;
                OS << ", " << std::endl;
                OS << "label " << irbranch->Operands[1].get()->getName() << std::endl;
            } else {
                OS << "label " << irbranch->Operands[0].get()->getName() << std::endl;
            }
            break;

        // Standard binary operators...
        case Add:
            BinaryLogicalIRInstPrint(OS, true);
            break;
        case Sub:
            BinaryLogicalIRInstPrint(OS, true);
        case Mul:
            BinaryLogicalIRInstPrint(OS, true);
            break;
        case Div:
            BinaryLogicalIRInstPrint(OS, true);
            break;
        case Rem:
            BinaryLogicalIRInstPrint(OS, false);
            break;

        // Logical operators...
        case And:
            BinaryLogicalIRInstPrint(OS, false);
            break;
        case Or:
            BinaryLogicalIRInstPrint(OS, false);
        case Xor:
            BinaryLogicalIRInstPrint(OS, false);
            break;

        // SetCC operators...
        case SetLE:
            SetCCIRInstPrint(OS);
            break;
        case SetGE:
            SetCCIRInstPrint(OS);
            break;
        case SetLT:
            SetCCIRInstPrint(OS);
            break;
        case SetGT:
            SetCCIRInstPrint(OS);
            break;
        case SetEQ:
            SetCCIRInstPrint(OS);
            break;
        case SetNE:
            SetCCIRInstPrint(OS);
            break;

        // Memory instructions...
        case Alloca:
            //instruction begin
            this->printPrefixName(OS);
            OS << " = ";
            OS << this->getOpcodeName() << "  " ;//打印alloc

            IRSequentialType * allocType;
            allocType = dynamic_cast<IRSequentialType *>(this->getType());
            if(allocType->getPrimitiveID() == IRType::PointerTyID){
                allocType->getElementType()->print(OS);
            }else if(allocType->getPrimitiveID() == IRType::ArrayTyID){
                allocType->print(OS);
            }
            // 回退一个字符
            OS.seekp(static_cast<std::streampos>(static_cast<std::streamoff>(OS.tellp()) - 1));
            //dynamic_cast<IRPointerType *>(this->getType())->print(OS);
            //打印对应的prmitive type而不是他本身的pointer type

            dynamic_cast<IRSequentialType *>(this->getType())->IRpointerPrintAlign(OS);//打印align
            //这里的alloca一定是一个IRPointer
            break;
        case Load:
            //instruction begin
            this->printPrefixName(OS);
            OS << this->getOpcodeName() << " " << std::endl;//打印load
            this->getType()->print(OS);                     //打印type

            dynamic_cast<IRLoadInst *>(const_cast<IRInstruction *>(this))->getPointerOperand()->getType()->print(OS);
            break;
        case Store:
            //instruction begin
            OS << this->getOpcodeName() << "  " ;//打印store

            operand1 = this->getOperand(0);
            /******如果是一个Constant，那么可以直接调用打印方法******/
            /******如果不是一个Constant，就必须先调用type的打印方法，然后调用printPrefixName()******/
            switch (operand1->getValueType()) {
                case IRValue::ConstantVal:
                    operand1->print(OS);
                    break;
                case IRValue::ArgumentVal:
                    operand1->getType()->print(OS);//打印type
                    operand1->printPrefixName(OS); //打印本名
                    break;
                case IRValue::InstructionVal:
                    operand1->getType()->print(OS);
                    operand1->printPrefixName(OS);
                    break;
            }
            //这里只可能是这三种情况，globalvarible不可能，因为它是指针

            // /******这里必然是IRPointerType******/
            OS << "  " ;
            operand2 = dynamic_cast<IRStoreInst *>(const_cast<IRInstruction *>(this))->getPointerOperand();
            operand2->getType()->print(OS);
            operand2->printPrefixName(OS);
            break;
        case Memcpy:
        //instruction begin
            OS << this->getOpcodeName() << "  " ;//打印store

            operand1 = dynamic_cast<IRMemcpyInst *>(const_cast<IRInstruction *>(this))->getSrcPointerOperand();
            operand1->getType()->print(OS);
            operand1->printPrefixName(OS);

            // /******这里必然是IRPointerType******/
            OS << "  " ;
            operand2 = dynamic_cast<IRMemcpyInst *>(const_cast<IRInstruction *>(this))->getDestPointerOperand();
            operand2->getType()->print(OS);
            operand2->printPrefixName(OS);
            break;


        // Other instructions...
        case PHI:
            //instruction begin
            this->printPrefixName(OS);//打印instructionName
            OS << " = " << std::endl;
            OS << this->getOpcodeName() << " " << std::endl;//打印phi

            const IRPHINode *irphinode;
            irphinode = dynamic_cast<const IRPHINode *>(this);
            for (unsigned i = 0; i < irphinode->getNumIncomingValues(); i++) {
                OS << "[" << std::endl;
                irphinode->getIncomingValue(i)->printPrefixName(OS);//打印赋值
                OS << ", " << std::endl;
                irphinode->getIncomingBlock(i)->printPrefixName(OS);//打印basicblock
                OS << "] " << std::endl;
            }
            break;
        case Call:
            //instruction begin
            this->printPrefixName(OS);//打印instructionName
            OS << " = " << std::endl;
            OS << this->getOpcodeName() << " " << std::endl;//打印call

            dynamic_cast<IRCallInst *>(const_cast<IRInstruction *>(this))->getCalledFunction()->print(OS);
            //打印出对应的call的函数
            break;
        case Shl:
            //instruction begin
            this->printPrefixName(OS);//打印instructionName
            OS << " = " << std::endl;
            OS << this->getOpcodeName() << " " << std::endl;//打印shl
            this->getType()->print(OS);                     //打印type

            //打印两个操作数，这两个操作数都是以primitiveType的形式出现，不会是derivedType
            for (unsigned i = 0; i < 2; i++) {
                operand = this->getOperand(i);
                switch (operand1->getValueType()) {
                    case IRValue::ConstantVal:
                        operand1->print(OS);
                        break;
                    case IRValue::ArgumentVal:
                        operand1->printPrefixName(OS);//打印本名
                        break;
                    case IRValue::InstructionVal:
                        operand1->printPrefixName(OS);
                        break;
                }
            }
            break;
        case Shr:
            this->printPrefixName(OS);//打印instructionName
            OS << " = " << std::endl;
            OS << this->getOpcodeName() << " " << std::endl;//打印shl
            this->getType()->print(OS);                     //打印type

            //打印两个操作数，这两个操作数都是以primitiveType的形式出现，不会是derivedType
            for (unsigned i = 0; i < 2; i++) {
                operand = this->getOperand(i);
                switch (operand1->getValueType()) {
                    case IRValue::ConstantVal:
                        operand1->print(OS);
                        break;
                    case IRValue::ArgumentVal:
                        operand1->printPrefixName(OS);//打印本名
                        break;
                    case IRValue::InstructionVal:
                        operand1->printPrefixName(OS);
                        break;
                }
            }
            break;

        default:
            throw "<Invalid operator> ";
    }
    OS << std::endl;
    // TODO
}
