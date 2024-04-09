#ifndef COMPILER_IRGLOBALVARIABLE_H
#define COMPILER_IRGLOBALVARIABLE_H
#include <cassert>

#include "IRGlobalValue.h"
#include "IRModule.h"

class IRModule;

class IRGlobalVariable : public IRGlobalValue {
    void setParent(IRModule *parent);

    bool isConstantGlobal;

public:
    IRGlobalVariable(IRType *Ty, bool isConstant, LinkageTypes Linkage,
                     IRConstant *Initializer = nullptr, const std::string &Name = "",
                     IRModule *Parent = nullptr);

    inline bool hasInitializer() const { return !Operands.empty(); }

    inline IRConstant *getInitializer() const {
        assert(hasInitializer() && "GV doesn't have initializer!");
        return (IRConstant *) Operands[0].val;
    }

    inline void setInitializer(IRConstant *CPV) {
        if (CPV == nullptr) {
            if (hasInitializer()) Operands.pop_back();
        } else {
            if (!hasInitializer()) Operands.emplace_back(nullptr, this);
            Operands[0].val = (IRValue *) CPV;
        }
    }

    bool isConstant() const { return isConstantGlobal; }

    void print(std::ostream &OS) const override;

    static inline bool classof(const IRGlobalVariable *) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == IRValue::GlobalVariableVal;
    }
};

#endif//COMPILER_IRGLOBALVARIABLE_H
