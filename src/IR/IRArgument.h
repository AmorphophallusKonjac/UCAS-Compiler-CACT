#ifndef COMPILER_IRARGUMENT_H
#define COMPILER_IRARGUMENT_H

#include "IRFunction.h"
#include "IRValue.h"

class IRValue;
class IRFunction;

class IRArgument : public IRValue {
    IRFunction *Parent = nullptr;

public:
    explicit IRArgument(IRType *Ty, const std::string &Name = "", IRFunction *F = nullptr);
    ~IRArgument() override = default;

    /******IRArgument的母函数******/
    void setParent(IRFunction *parent);
    inline IRFunction *getParent() { return Parent; }

    /******IRArgument的print方法******/
    void print(std::ostream &OS) const override;

    /******classof方法，判断从父类下来的子类是不是对应的IRArgument类******/
    static inline bool classof(const IRArgument *) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == ArgumentVal;
    }
};


#endif//COMPILER_IRARGUMENT_H
