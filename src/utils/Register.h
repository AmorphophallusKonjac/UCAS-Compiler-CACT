#ifndef COMPILER_REGISTER_H
#define COMPILER_REGISTER_H
#pragma once
#include <vector>
#include <string>

class Register{
public:
    enum RegTy{
        CallerSaved,    //t系列寄存器
        CalleeSaved,    //s系列寄存器
        Param,          //a系列寄存器
    };

    /*获得reg系列及其编号*/
    virtual const std::string& getRegName() = 0;

    /*获得在所有reg中的序号*/
    virtual unsigned getRegSeq() = 0;

    /*获得reg中的类型*/
    virtual RegTy getRegty() = 0;
};

class CallerSavedRegister : public Register{
private:
    static std::vector<CallerSavedRegister*> CallerSavedvec;
    static const unsigned RegMAXNum = 7;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy    regty = CallerSaved;     //寄存器的类型

public:
    explicit CallerSavedRegister(unsigned num) : regNum(num) 
    { if(num <= 2){ regSeq = num+5;  }
      else        { regSeq = num+25; } 
      regName = "t" + std::to_string(regNum); };

    static void initTreg(){
        for(unsigned i=0; i<RegMAXNum;i++){
            CallerSavedvec.push_back(new CallerSavedRegister(i));
        }
    }

    const std::string& getRegName(){
        return regName;
    };

    unsigned getRegSeq(){
        return regSeq;
    };

    RegTy getRegty(){
        return regty;
    };

};

class CalleeSavedRegister : public Register{
private:
    static std::vector<CalleeSavedRegister*> CalleeSavedvec;
    static const unsigned RegMAXNum = 12;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy    regty = CalleeSaved;     //寄存器的类型

public:
    explicit CalleeSavedRegister(unsigned num) : regNum(num) 
    { if(num <= 1){ regSeq = num+8;  }
      else        { regSeq = num+16; } 
      regName = "t" + std::to_string(regNum); };

    static void initSreg(){
        for(unsigned i=0; i<RegMAXNum;i++){
            CalleeSavedvec.push_back(new CalleeSavedRegister(i));
        }
    }

    const std::string& getRegName(){
        return regName;
    };

    unsigned getRegSeq(){
        return regSeq;
    };

    RegTy getRegty(){
        return regty;
    };
};

class ParamRegister : public Register{
private:
    static std::vector<ParamRegister*> Paramvec;
    static const unsigned RegMAXNum = 8;

    unsigned regNum;    //系列中寄存器的编号
    unsigned regSeq;    //所有寄存器中的编号
    std::string regName;//寄存器的名字
    RegTy    regty = Param;     //寄存器的类型

public:
    explicit ParamRegister(unsigned num) : regNum(num) 
    { regSeq = num+10; 
      regName = "t" + std::to_string(regNum); };

    static void initAreg(){
        for(unsigned i=0; i<RegMAXNum;i++){
            Paramvec.push_back(new ParamRegister(i));
        }
    }

    const std::string& getRegName(){
        return regName;
    };

    unsigned getRegSeq(){
        return regSeq;
    };

    RegTy getRegty(){
        return regty;
    };
};

class RegisterFactory{
public:
    static void initReg(){
        CallerSavedRegister::initTreg();
        CalleeSavedRegister::initSreg();
        ParamRegister::initAreg();
    }
};

#endif //COMPILER_REGISTER_H