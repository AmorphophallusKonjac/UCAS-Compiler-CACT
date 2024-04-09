#include "IRConstant.h"

IRConstantBool::IRConstantBool(bool V) : IRConstant(IRType::BoolTy) {
    Val = V;
}

IRConstantBool *IRConstantBool::True = new IRConstantBool(true);
IRConstantBool *IRConstantBool::False = new IRConstantBool(false);
IRConstantInt *IRConstantInt::Null = new IRConstantInt(0);
IRConstantInt *IRConstantInt::AllOnes = new IRConstantInt(-1);
IRConstantFloat *IRConstantFloat::Null = new IRConstantFloat(0);
IRConstantDouble *IRConstantDouble::Null = new IRConstantDouble(0);

void IRConstant::print(std::ostream &O) const {
    // TODO
}

IRConstant *IRConstant::getNullValue(const IRType *Ty) {
    switch (Ty->getPrimitiveID()) {
        case IRType::IntTyID:
            return IRConstantInt::Null;
        case IRType::FloatTyID:
            return IRConstantFloat::Null;
        case IRType::DoubleTyID:
            return IRConstantDouble::Null;
        case IRType::BoolTyID:
            return IRConstantBool::False;
    }
    return nullptr;
}
IRConstant *IRConstant::getAllOnesValue(const IRType *Ty) {
    switch (Ty->getPrimitiveID()) {
        case IRType::IntTyID:
            return IRConstantInt::AllOnes;
        case IRType::BoolTyID:
            return IRConstantBool::True;
    }
    return nullptr;
}

IRConstantInt::IRConstantInt(int V) : IRConstant(IRType::IntTy) {
    Val = V;
}

IRConstantFloat::IRConstantFloat(float V) : IRConstant(IRType::FloatTy) {
    Val = V;
}

IRConstantDouble::IRConstantDouble(double V) : IRConstant(IRType::DoubleTy) {
    Val = V;
}

IRConstantArray::IRConstantArray(IRArrayType *ty, std::vector<IRConstant *> V)
    : IRConstant(dynamic_cast<IRType *>(ty)) {
    arrayTy = ty;
    for (auto val: V) {
        Operands.emplace_back(val, this);
    }
}
