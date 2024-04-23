#ifndef COMPILER_IRCONSTANT_H
#define COMPILER_IRCONSTANT_H


#include <any>
#include <map>

#include "IRDerivedTypes.h"
#include "IRUser.h"

/******这里是一系列IRConstant的子类，如果往里面传入一个val值，那么可以直接获得一个IRConstant子类的静态对象******/
class IRConstant : public IRUser {
protected:
    inline explicit IRConstant(IRType *Ty) : IRUser(Ty, IRValue::ConstantVal) {}

public:
    static IRConstant *getNullValue(const IRType *Ty);
    static IRConstant *getAllOnesValue(const IRType *Ty);

    void print(std::ostream &OS) const override;
    void printPrefixName(std::ostream &OS) const override ;
};

class IRConstantBool : public IRConstant {
    bool Val;

public:
    explicit IRConstantBool(bool V);

    /******静态的true和false对象******/
    static IRConstantBool *True, *False;// The True & False values

    /******IRConstantBool的get方法，通过判断给定的真实的bool值，返回一个IRConstant的对象******/
    static IRConstantBool *get(bool Value) { return Value ? True : False; }
    static IRConstantBool *get(const IRType *Ty, bool Value) { return get(Value); }

    /******IRConstantBool的judge方法，可以直接发挥一个true或false的IRConstant对象******/
    inline IRConstantBool *inverted() const { return (this == True) ? False : True; }

    inline bool getRawValue() const { return Val; }
};

class IRConstantInt : public IRConstant {
    int Val;
    static std::map<int, IRConstantInt *> constantMap;

protected:
    /******构造******/
    explicit IRConstantInt(int V);

public:
    static IRConstantInt *Null, *AllOnes;

    /**
     * 构造 IRConstantInt 的工厂方法，返回一个创建好的对象指针
     * @param V 常数的值
     * @return 创建好的对象的指针
     */
    static IRConstantInt *get(int V);

    /******返回val******/
    inline int getRawValue() const { return Val; }
};

class IRConstantFloat : public IRConstant {
    float Val;
    static std::map<float, IRConstantFloat *> constantMap;

protected:
    /******构造******/
    explicit IRConstantFloat(float V);

public:
    static IRConstantFloat *Null;

    /**
     * 构造 IRConstantFloat 的工厂方法，返回一个创建好的对象指针
     * @param V 常数的值
     * @return 创建好的对象的指针
     */
    static IRConstantFloat *get(float V);
    /******返回val******/
    inline float getRawValue() const { return Val; }
};

class IRConstantDouble : public IRConstant {
    double Val;
    static std::map<double, IRConstantDouble *> constantMap;

protected:
    /******构造******/
    explicit IRConstantDouble(double V);

public:
    static IRConstantDouble *Null;

    /**
     * 构造 IRConstantFloat 的工厂方法，返回一个创建好的对象指针
     * @param V 常数的值
     * @return 创建好的对象的指针
     */
    static IRConstantDouble *get(double V);
    /******返回val******/
    inline double getRawValue() const { return Val; }
};

class IRConstantArray : public IRConstant {
    IRArrayType *arrayTy;

public:
    /******构造******/
    IRConstantArray(IRArrayType *ty, const std::vector<IRConstant *> &V);

    /******强转类型。上面的子类都是primitiveType，这里的类是arrayType，与globalvarible那里强转pointertype同理******/
    inline const IRArrayType *getType() const {
        return (IRArrayType *) IRValue::getType();
    }
    /******返回val******/
    inline const std::vector<IRUse> &getValues() const { return Operands; }
};


#endif//COMPILER_IRCONSTANT_H
