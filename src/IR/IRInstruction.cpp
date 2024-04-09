#include "IRInstruction.h"
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
void IRInstruction::print(std::ostream &OS) const {
    // TODO
}
