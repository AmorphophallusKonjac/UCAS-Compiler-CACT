#include "IRType.h"

#include <iostream>
#include <map>

#include "IRDerivedTypes.h"
#include <iostream>

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
            return 8; // TODO solve TypeTy size
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

void IRType::print(std::ostream &OS) const {
    if(this->isPrimitiveType()){//证明这个类本身就是int或者double等而不是指针类型，因此getType直接获得的就是Type的静态对象
        OS << this->getPrimitiveType(this->getPrimitiveID())->getName() << std::endl;//用ID返回TYPE
        //每一个type都是再通过判断是不是primitive之后，又重新指向一个已经被静态定义好的static type对象\
        //感觉这里因为是一个this->getType()静态对象，因此可以直接
        //this->getType()->getName()?
    }else if(this->isDerivedType()){
        switch (this->getPrimitiveID()) {//这里是可以通过primitiveID来判断他是属于哪个具体类的
            case IRType::FunctionTyID:
                OS << dynamic_cast<const IRFunctionType*>(this)->getReturnType()->getName() << std::endl;
                break;
            case IRType::ArrayTyID:
                const IRArrayType* arraytype;
                arraytype = dynamic_cast<const IRArrayType*>(this);
                OS << "[ " << arraytype->getNumElements() << " x " <<arraytype->getElementType()->getName() << "]" << std::endl;
                break;
                /*@global_array = [2 x [2 x double]] = @global_array = [4 x double]
                %1 = alloca [2 x [2 x double]] = */
            case IRType::PointerTyID:
                OS << dynamic_cast<const IRPointerType*>(this)->getElementType()->getName() << std::endl;
                OS << "*" << std::endl;//指针加一个*
                break;
        }

    }else{
        throw std::runtime_error("PrimitiveID error!");
    }
    OS << " " << std::endl;
    //感觉其实不需要his->getType()->getPrimitiveType(this->getType()->getPrimitiveID())对ID进行解析？？？
    //因为每次这样做之后返回的直接就是一个静态的type对象，并不需要对他的ID进行解析
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

void IRPointerType::IRpointerPrintAlign(std::ostream &OS) {
    switch (this->getElementType()->getPrimitiveID()) {
        case IRType::BoolTyID:
            OS << " align 4 " << std::endl;
            break;
        case IRType::IntTyID:
            OS << " align 4 " << std::endl;
            break;
        case IRType::FloatTyID:
            OS << " align 4 " << std::endl;
            break;
        case IRType::DoubleTyID:    
            OS << " align 8 " << std::endl;
            break;
    }
}
