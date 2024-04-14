#include "IRConstant.h"

#include <iostream>
#include <map>

IRConstantBool::IRConstantBool(bool V) : IRConstant(IRType::BoolTy) {
    Val = V;
}

IRConstantBool *IRConstantBool::True = new IRConstantBool(true);
IRConstantBool *IRConstantBool::False = new IRConstantBool(false);
IRConstantInt *IRConstantInt::Null = new IRConstantInt(0);
IRConstantInt *IRConstantInt::AllOnes = new IRConstantInt(-1);
IRConstantFloat *IRConstantFloat::Null = new IRConstantFloat(0);
IRConstantDouble *IRConstantDouble::Null = new IRConstantDouble(0);

std::map<int, IRConstantInt *> IRConstantInt::constantMap;
std::map<float, IRConstantFloat *> IRConstantFloat::constantMap;
std::map<double, IRConstantDouble *> IRConstantDouble::constantMap;

void IRConstant::print(std::ostream &OS) const {
    this->getType()->print(OS);

    /******根据IRConstant中的type类型来打印出它的value值******/
    switch (this->getType()->getPrimitiveID()) {
        case IRType::IntTyID:
            OS << dynamic_cast<const IRConstantInt *>(this)->getRawValue() << std::endl;
            break;
        case IRType::FloatTyID:
            OS << dynamic_cast<const IRConstantFloat *>(this)->getRawValue() << std::endl;
            break;
        case IRType::DoubleTyID:
            OS << dynamic_cast<const IRConstantDouble *>(this)->getRawValue() << std::endl;
            break;
        case IRType::BoolTyID:
            OS << dynamic_cast<const IRConstantBool *>(this)->getRawValue() << std::endl;
            break;
    }
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

IRConstantInt *IRConstantInt::get(int V) {
    if (V == 0) {
        return IRConstantInt::Null;
    } else if (V == -1) {
        return IRConstantInt::AllOnes;
    } else if (auto ret = constantMap[V]) {
        return ret;
    } else {
        ret = new IRConstantInt(V);
        constantMap[V] = ret;
        return ret;
    }
}

IRConstantFloat::IRConstantFloat(float V) : IRConstant(IRType::FloatTy) {
    Val = V;
}
IRConstantFloat *IRConstantFloat::get(float V) {
    if (V == 0) {
        return IRConstantFloat::Null;
    } else if (auto ret = constantMap[V]) {
        return ret;
    } else {
        ret = new IRConstantFloat(V);
        constantMap[V] = ret;
        return ret;
    }
}

IRConstantDouble::IRConstantDouble(double V) : IRConstant(IRType::DoubleTy) {
    Val = V;
}
IRConstantDouble *IRConstantDouble::get(double V) {
    if (V == 0) {
        return IRConstantDouble::Null;
    } else if (auto ret = constantMap[V]) {
        return ret;
    } else {
        ret = new IRConstantDouble(V);
        constantMap[V] = ret;
        return ret;
    }
}

IRConstantArray::IRConstantArray(IRArrayType *ty, const std::vector<IRConstant *> &V)
    : IRConstant(dynamic_cast<IRType *>(ty)) {
    arrayTy = ty;
    for (auto val: V) {
        Operands.emplace_back(val, this);
    }
}
