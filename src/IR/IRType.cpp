#include "IRType.h"

#include <iostream>
#include <map>

#include "IRDerivedTypes.h"

static std::map<const IRType *, std::string> ConcreteTypeDescriptions;

IRType::IRType(const std::string &Name, IRType::PrimitiveID id)
    : IRValue(IRType::TypeTy, IRValue::TypeVal) {
    if (!Name.empty()) {
        ConcreteTypeDescriptions[this] = Name;
    }
    ID = id;
}

const IRType *IRType::getPrimitiveType(IRType::PrimitiveID IDNumber) {
    switch (IDNumber) {
        case VoidTyID:
            return VoidTy;
        case BoolTyID:
            return BoolTy;
        case IntTyID:
            return IntTy;
        case FloatTyID:
            return FloatTy;
        case DoubleTyID:
            return DoubleTy;
        case TypeTyID:
            return TypeTy;
        case LabelTyID:
            return LabelTy;
        default:
            return nullptr;
    }
}
unsigned IRType::getPrimitiveSize() const {
    switch (getPrimitiveID()) {
        case BoolTyID:
            return 1;
        case IntTyID:
            return 4;
        case FloatTyID:
            return 4;
        case DoubleTyID:
            return 8;
        case TypeTyID:
            return 8;// TODO solve TypeTy size
        default:
            return 0;
    }
}


//===----------------------------------------------------------------------===//
//                           Auxiliary classes
//===----------------------------------------------------------------------===//
//
// These classes are used to implement specialized behavior for each different
// type.
//
class IRSignedIntType : public IRType {
public:
    IRSignedIntType(const std::string &Name, PrimitiveID id) : IRType(Name, id) {}

    bool isInteger() const override { return true; }
};

class IROtherType : public IRType {
public:
    IROtherType(const std::string &N, PrimitiveID id) : IRType(N, id) {}
};

static struct TypeType : public IRType {
    TypeType() : IRType("type", TypeTyID) {}
} TheTypeTy;// Implement the type that is global.

//===----------------------------------------------------------------------===//
//                           Static 'Type' data
//===----------------------------------------------------------------------===//

static IROtherType TheVoidTy("void", IRType::VoidTyID);
static IROtherType TheBoolTy("bool", IRType::BoolTyID);
static IRSignedIntType TheIntTy("int", IRType::IntTyID);
static IROtherType TheFloatTy("float", IRType::FloatTyID);
static IROtherType TheDoubleTy("double", IRType::DoubleTyID);
static IROtherType TheLabelTy("label", IRType::LabelTyID);

IRType *IRType::VoidTy = &TheVoidTy;
IRType *IRType::BoolTy = &TheBoolTy;
IRType *IRType::IntTy = &TheIntTy;
IRType *IRType::FloatTy = &TheFloatTy;
IRType *IRType::DoubleTy = &TheDoubleTy;
IRType *IRType::TypeTy = &TheTypeTy;
IRType *IRType::LabelTy = &TheLabelTy;

void IRType::print(std::ostream &O) const {
    // TODO add print
}


IRFunctionType::IRFunctionType(IRType *Result, const std::vector<IRType *> &Params)
    : IRType("", IRType::FunctionTyID),
      paramTys(Params) {
    resultType = Result;
}
IRArrayType::IRArrayType(IRType *ElType, unsigned int NumEl)
    : IRSequentialType(IRType::ArrayTyID, ElType) {
    NumElements = NumEl;
}
IRPointerType::IRPointerType(IRType *ElType)
    : IRSequentialType(IRType::PointerTyID, ElType) {
}
