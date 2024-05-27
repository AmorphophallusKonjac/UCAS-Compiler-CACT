#ifndef COMPILER_LIVEVARIABLE_H
#define COMPILER_LIVEVARIABLE_H
#pragma once

#include "IR/IRInstruction.h"
#include "IR/IRBasicBlock.h"
#include "IR/IRValue.h"
#include <vector>

class IRBasicBlock;

class IRInstruction;

class LiveVariable {
private:
    std::vector<IRValue*> INLiveList;
    std::vector<IRValue*> OUTLiveList;

public:
    LiveVariable(){};

    ~LiveVariable() = default;

    std::vector<IRValue*>* getINLive() { return &INLiveList; };

    std::vector<IRValue*>* getOUTLive(){ return &OUTLiveList; };

    static void genLiveVariable(IRFunction *F);

};

class LiveVariableBB : public LiveVariable {
private:
    IRBasicBlock* BB;

public:
    LiveVariableBB(IRBasicBlock* BB) : BB(BB) {};

    ~LiveVariableBB() = default;

    static void genLiveVariableBB(IRFunction *F);
};

class LiveVariableInst : public LiveVariable {
private:
    IRInstruction* Inst;

public:
    LiveVariableInst(IRInstruction* Inst) : Inst(Inst) {};

    ~LiveVariableInst() = default;
    
    static void genLiveVariableInst(IRBasicBlock *BB);
};


#endif //COMPILER_LIVEVARIABLE_H
