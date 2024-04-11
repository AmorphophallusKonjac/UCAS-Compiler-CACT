#include "IRInstruction.h"
#include "IR/IRBasicBlock.h"
#include "iTerminators.h"
#include <iostream>
IRInstruction::IRInstruction(IRType *Ty, unsigned int iType, const std::string &Name, IRInstruction *InsertBefore)
    : IRUser(Ty, InstructionVal, Name) {
    Parent = nullptr;
    this->iType = iType;

    if (InsertBefore) {
        InsertBefore->getParent()->addInstruction(this);
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
void IRInstruction::print(std::ostream &OS) const {
    switch (getOpcode()) {
        // Terminators
        case Ret:
            OS << this->getOpcodeName() << " " << std::endl;//打印ret
            this->Operands[0].val->print(OS);//打印一个IRconstant
            break;
        case Br:
            OS << this->getOpcodeName() << " " << std::endl;//打印ret
            const IRBranchInst* irbranch;
            irbranch = dynamic_cast<const IRBranchInst*>(this);
            if(irbranch->isConditional()){//有条件判断
                //打印一个IRvalue
                irbranch->Operands[2].val->print(OS);
                //打印两个label跳转标号
                OS << ", " << std::endl;
                OS << "label " << irbranch->Operands[0].val->getName() <<std::endl;
                OS << ", " << std::endl;
                OS << "label " << irbranch->Operands[1].val->getName() <<std::endl;
            }else{
                OS << "label " << irbranch->Operands[0].val->getName() <<std::endl;
            }
            break;

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
    // TODO
}
