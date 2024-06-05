#ifndef COMPILER_REGISTER_H
#define COMPILER_REGISTER_H
#pragma once

#include <vector>
#include <string>

class Register {
public:
    enum RegTy {
        CallerSaved,    //t系列寄存器
        CalleeSaved,    //s系列寄存器
        Param,          //a系列寄存器
    };

    /*获得reg系列及其编号*/
    virtual const std::string &getRegName() = 0;

    /*获得在所有reg中的序号*/
    virtual unsigned getRegSeq() = 0;

    /*获得reg中的类型*/
    virtual RegTy getRegty() = 0;
};

class CallerSavedRegister : public Register {
private:
    static std::vector<CallerSavedRegister *> CallerSavedvec;
    static const unsigned RegMAXNum = 7;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy regty = CallerSaved;     //寄存器的类型

public:
    explicit CallerSavedRegister(unsigned num) : regNum(num) {
        if (num <= 2) { regSeq = num + 5; }
        else { regSeq = num + 25; }
        regName = "t" + std::to_string(regNum);
    };

    /*初始化静态reg对象，并获得静态reg对象List*/
    static void initTreg() {
        for (unsigned i = 0; i < RegMAXNum; i++) {
            CallerSavedvec.push_back(new CallerSavedRegister(i));
        }
    }

    static std::vector<CallerSavedRegister *> &getTregList() {
        return CallerSavedvec;
    }

    /*获得reg属性*/
    const std::string &getRegName() { return regName; };

    unsigned getRegSeq() { return regSeq; };

    RegTy getRegty() { return regty; };

};

class CalleeSavedRegister : public Register {
private:
    static std::vector<CalleeSavedRegister *> CalleeSavedvec;
    static const unsigned RegMAXNum = 12;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy regty = CalleeSaved;     //寄存器的类型

public:
    explicit CalleeSavedRegister(unsigned num) : regNum(num) {
        if (num <= 1) { regSeq = num + 8; }
        else { regSeq = num + 16; }
        regName = "s" + std::to_string(regNum);
    };

    /*初始化静态reg对象，并获得静态reg对象List*/
    static void initSreg() {
        for (unsigned i = 0; i < RegMAXNum; i++) {
            CalleeSavedvec.push_back(new CalleeSavedRegister(i));
        }
    }

    static std::vector<CalleeSavedRegister *> &getSregList() {
        return CalleeSavedvec;
    }

    /*获得reg属性*/
    const std::string &getRegName() { return regName; };

    unsigned getRegSeq() { return regSeq; };

    RegTy getRegty() { return regty; };
};

class ParamRegister : public Register {
private:
    static std::vector<ParamRegister *> Paramvec;
    static const unsigned RegMAXNum = 8;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy regty = Param;     //寄存器的类型

public:
    explicit ParamRegister(unsigned num) : regNum(num) {
        regSeq = num + 10;
        regName = "a" + std::to_string(regNum);
    };

    /*初始化静态reg对象，并获得静态reg对象List*/
    static void initAreg() {
        for (unsigned i = 0; i < RegMAXNum; i++) {
            Paramvec.push_back(new ParamRegister(i));
        }
    }

    static std::vector<ParamRegister *> &getAregList() {
        return Paramvec;
    }

    /*获得reg属性*/
    const std::string &getRegName() { return regName; };

    unsigned getRegSeq() { return regSeq; };

    RegTy getRegty() { return regty; };
};

class RegisterFactory {
private:
    static std::vector<Register *> RegList;

public:

    /*初始化所有静态reg对*/
    static void initReg() {
        CallerSavedRegister::initTreg();
        CalleeSavedRegister::initSreg();
        ParamRegister::initAreg();

        RegList.assign(CallerSavedRegister::getTregList().begin(), CallerSavedRegister::getTregList().end());
        RegList.assign(CalleeSavedRegister::getSregList().begin(), CalleeSavedRegister::getSregList().end());
        RegList.assign(ParamRegister::getAregList().begin(), ParamRegister::getAregList().end());
    }

    /*获得所有静态reg对象List*/
    static std::vector<Register *> &getRegList() {
        return RegList;
    }
};

#endif //COMPILER_REGISTER_H