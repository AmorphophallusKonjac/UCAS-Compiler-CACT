#include "iMemory.h"

#include "IRConstant.h"
#include "IRDerivedTypes.h"

IRAllocationInst::IRAllocationInst(IRType *Ty, IRValue *ArraySize, unsigned int iTy, const std::string &Name, IRInstruction *InsertBefore)
    : IRInstruction(new IRPointerType(Ty), iTy, Name, InsertBefore) {
    if (!ArraySize) ArraySize = new IRConstantInt(1);
    Operands.reserve(1);
    Operands.emplace_back(ArraySize, this);
}

bool IRAllocationInst::isArrayAllocation() const {
    return dynamic_cast<IRConstantInt *>(getOperand(0))->getRawValue() != 1;
}

const IRType *IRAllocationInst::getAllocatedType() const {
    return getType()->getElementType();
}


IRLoadInst::IRLoadInst(IRValue *Ptr, const std::string &Name, IRInstruction *InsertBefore)
    : IRInstruction(dynamic_cast<IRPointerType *>(Ptr->getType())->getElementType(),
                    Load, Name, InsertBefore),
      Volatile(false) {
    Operands.reserve(1);
    Operands.emplace_back(Ptr, this);
}

IRLoadInst::IRLoadInst(IRValue *Ptr, const std::string &Name, bool isVolatile, IRInstruction *InsertBefore)
    : IRInstruction(dynamic_cast<IRPointerType *>(Ptr->getType())->getElementType(),
                    Load, Name, InsertBefore),
      Volatile(isVolatile) {
    Operands.reserve(1);
    Operands.emplace_back(Ptr, this);
}

IRStoreInst::IRStoreInst(IRValue *Val, IRValue *Ptr, IRInstruction *InsertBefore)
    : IRInstruction(IRType::VoidTy, Store, "", InsertBefore), Volatile(false) {
    Operands.reserve(2);
    Operands.emplace_back(Val, this);
    Operands.emplace_back(Ptr, this);
}
IRStoreInst::IRStoreInst(IRValue *Val, IRValue *Ptr, bool isVolatile, IRInstruction *InsertBefore)
    : IRInstruction(IRType::VoidTy, Store, "", InsertBefore), Volatile(isVolatile) {
    Operands.reserve(2);
    Operands.emplace_back(Val, this);
    Operands.emplace_back(Ptr, this);
}
