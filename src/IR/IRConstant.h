#ifndef COMPILER_IRCONSTANT_H
#define COMPILER_IRCONSTANT_H


#include "IRDerivedTypes.h"
#include "IRUser.h"

class IRConstant : public IRUser {
protected:
    inline explicit IRConstant(IRType *Ty) : IRUser(Ty, IRValue::ConstantVal) {}

public:
    static IRConstant *getNullValue(const IRType *Ty);
    static IRConstant *getAllOnesValue(const IRType *Ty);

    void print(std::ostream &O) const override;
};

class IRConstantBool : public IRConstant {
    bool Val;

public:
    explicit IRConstantBool(bool V);

    static IRConstantBool *True, *False;// The True & False values

    static IRConstantBool *get(bool Value) { return Value ? True : False; }
    static IRConstantBool *get(const IRType *Ty, bool Value) { return get(Value); }

    inline IRConstantBool *inverted() const { return (this == True) ? False : True; }

    inline bool getValue() const { return Val; }
};

class IRConstantInt : public IRConstant {
    int Val;

public:
    static IRConstantInt *Null, *AllOnes;

    explicit IRConstantInt(int V);
    inline int getRawValue() const { return Val; }
};

class IRConstantFloat : public IRConstant {
    float Val;

public:
    static IRConstantFloat *Null;
    explicit IRConstantFloat(float V);
    inline float getRawValue() const { return Val; }
};

class IRConstantDouble : public IRConstant {
    double Val;

public:
    static IRConstantDouble *Null;
    explicit IRConstantDouble(double V);
    inline double getRawValue() const { return Val; }
};

class IRConstantArray : public IRConstant {
    IRArrayType *arrayTy;

public:
    IRConstantArray(IRArrayType *ty, std::vector<IRConstant *> V);

    inline const IRArrayType *getType() const {
        return (IRArrayType *) IRValue::getType();
    }

    inline const std::vector<IRUse> &getValues() const { return Operands; }
};


#endif//COMPILER_IRCONSTANT_H
