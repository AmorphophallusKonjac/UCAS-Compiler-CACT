#include "GlobalVariable.h"
#include "Module.h"
#include "IR/IRGlobalVariable.h"
#include <iostream>
#include <cassert>
#include "IR/IRConstant.h"

namespace RISCV {
    GlobalVariable::GlobalVariable(IRGlobalVariable *irGV, Module *module) {
        this->irGV = irGV;
        name = irGV->getName();
        if (irGV->isConstant()) {
            section = Section::RODATA;
        } else {
            if (irGV->isIsinitial()) {
                section = Section::DATA;
            } else {
                section = Section::BSS;
            }
        }
        if (IRArrayType::classof(irGV->getType()->getElementType())) {
            // 是数组
            auto arrayType = dynamic_cast<IRArrayType *>(irGV->getType()->getElementType());
            size = arrayType->getNumElements() * (arrayType->getElementType()->getPrimitiveSize());
        } else {
            // 不是数组
            auto varType = irGV->getType()->getElementType();
            size = varType->getPrimitiveSize();
        }
        module->addGlobalVariable(this);
    }

    IRGlobalVariable *GlobalVariable::getIrGv() const {
        return irGV;
    }

    void GlobalVariable::print(std::ostream &O) {
        O << "\t.globl\t" << name << "_obj" << std::endl;
        O << "\t" << getSectionName() << std::endl;
        O << "\t.align\t" << align << std::endl;
        O << "\t.type\t" << name << "_obj" << ", @object" << std::endl;
        O << "\t.size\t" << size << std::endl;
        O << name << "_obj" << ":" << std::endl;
        printInitVal(O);
        O << std::endl;
    }

    std::string GlobalVariable::getSectionName() {
        switch (section) {
            case BSS:
                return ".bss";
            case DATA:
                return ".data";
            case RODATA:
                return ".rodata";
            default:
                assert(0 && "error section");
        }
    }

    void GlobalVariable::printInitVal(std::ostream &O) {
        auto initializer = irGV->getInitializer();
        switch (initializer->getType()->getPrimitiveID()) {
            case IRType::IntTyID:
                O << "\t.word " << dynamic_cast<IRConstantInt *>(initializer)->getRawValue() << std::endl;
                break;
            case IRType::FloatTyID:
                O << "\t.float " << dynamic_cast<IRConstantFloat *>(initializer)->getRawValue() << std::endl;
                break;
            case IRType::DoubleTyID:
                O << "\t.double " << dynamic_cast<IRConstantDouble *>(initializer)->getRawValue() << std::endl;
                break;
            case IRType::BoolTyID:
                O << "\t.byte " << dynamic_cast<IRConstantBool *>(initializer)->getRawValue() << std::endl;
                break;
            case IRType::ArrayTyID:
                unsigned zeroCount = 0;
                bool firstVal = true;
                auto size = dynamic_cast<IRConstantArray *>(initializer)->getValues()[0].get()->getType()->getPrimitiveSize();
                for (const auto &irUse: dynamic_cast<IRConstantArray *>(initializer)->getValues()) {
                    auto val = dynamic_cast<IRConstant *>(irUse.get());
                    if (val->jugdeZero(val)) {
                        if (zeroCount == 0 && !firstVal) {
                            // 回退2个字符
                            O.seekp(static_cast<std::streampos>(static_cast<std::streamoff>(O.tellp()) - 2));
                            O << std::endl;
                        }
                        ++zeroCount;
                    } else {
                        // process zero
                        if (zeroCount) {
                            O << "\t.zero " << size * zeroCount << std::endl;
                            zeroCount = 0;
                            printType(O, val->getType());
                        } else if (firstVal) {
                            printType(O, val->getType());
                        }
                        printVal(O, val);
                    }
                    firstVal = false;
                }
                if (zeroCount) {
                    O << "\t.zero " << size * zeroCount << std::endl;
                } else {
                    O.seekp(static_cast<std::streampos>(static_cast<std::streamoff>(O.tellp()) - 2));
                    O << std::endl;
                }
        }
    }

    void GlobalVariable::printType(std::ostream &O, IRType *type) {
        switch (type->getPrimitiveID()) {
            case IRType::IntTyID:
                O << "\t.word ";
                break;
            case IRType::FloatTyID:
                O << "\t.float ";
                break;
            case IRType::DoubleTyID:
                O << "\t.double ";
                break;
            case IRType::BoolTyID:
                O << "\t.byte ";
                break;
            default:
                assert(0 && "wtf");
        }
    }

    void GlobalVariable::printVal(std::ostream &O, IRConstant *val) {
        switch (val->getType()->getPrimitiveID()) {
            case IRType::IntTyID:
                O << dynamic_cast<IRConstantInt *>(val)->getRawValue() << ", ";
                break;
            case IRType::FloatTyID:
                O << dynamic_cast<IRConstantFloat *>(val)->getRawValue() << ", ";
                break;
            case IRType::DoubleTyID:
                O << dynamic_cast<IRConstantDouble *>(val)->getRawValue() << ", ";
                break;
            case IRType::BoolTyID:
                O << dynamic_cast<IRConstantBool *>(val)->getRawValue() << ", ";
                break;
            default:
                assert(0 && "wtf");
        }
    }
} // RISCV