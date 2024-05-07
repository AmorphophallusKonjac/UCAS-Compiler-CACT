//
// Created by yuanjunkang on 24-5-5.
//

#include "TemporaryVariable.h"


const std::any &TemporaryVariable::getValue() const {
    return value;
}

TemporaryVariable::tempVarType TemporaryVariable::getType() const {
    return type;
}

void TemporaryVariable::setValue(const std::any &value) {
    TemporaryVariable::value = value;
}

void TemporaryVariable::setType(TemporaryVariable::tempVarType type) {
    TemporaryVariable::type = type;
}

TemporaryVariable::TemporaryVariable(std::any value, TemporaryVariable::tempVarType type) : value(std::move(value)),
                                                                                                   type(type) {}

std::string TemporaryVariable::getTypeString(tempVarType ty) {
    switch (ty) {
        case Func:
            return "Func";
        case Int:
            return "Int";
        case Float:
            return "Float";
        case Double:
            return "Double";
        case Bool:
            return "Bool";
        case Void:
            return "Void";
        case Pointer:
            return "Pointer";
        default:
            return "Unknown";
    }
}

void TemporaryVariable::print() {
    auto ty = getType();
    printf("Temporary Variable Type = ");
    std::cout << getTypeString(ty);
    printf(" , value = ");
    switch (ty) {
        case Bool:{
            printf("%d\n", std::any_cast<bool>(getValue()));
            break;
        }
        case Int:{
            printf("%d\n", std::any_cast<int>(getValue()));
            break;
        }
        case Float:{
            printf("%f\n", std::any_cast<float>(getValue()));
            break;
        }
        case Double:{
            printf("%lf\n", std::any_cast<double>(getValue()));
            break;
        }
        case Pointer:{
            printf("%lx\n", std::any_cast<uint64_t>(getValue()));
            break;
        }
        default:{
            printf("NULL\n");
        }
    }
}

