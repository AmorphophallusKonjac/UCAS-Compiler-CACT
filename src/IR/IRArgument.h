#ifndef COMPILER_IRARGUMENT_H
#define COMPILER_IRARGUMENT_H

#include "IRFunction.h"
#include "IRValue.h"

class IRValue;
class IRFunction;

class IRArgument : public IRValue {
    IRFunction *Parent = nullptr;
    void setParent(IRFunction *parent);

public:
    explicit IRArgument(IRType *Ty, const std::string &Name = "", IRFunction *F = nullptr);
    ~IRArgument() override = default;

    inline IRFunction *getParent() { return Parent; }

    void print(std::ostream &OS) const override;

    static inline bool classof(const IRArgument *) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == ArgumentVal;
    }
};


#endif//COMPILER_IRARGUMENT_H
