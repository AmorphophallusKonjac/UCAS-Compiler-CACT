#ifndef COMPILER_BASICINDUCTIONVARIABLE_H
#define COMPILER_BASICINDUCTIONVARIABLE_H


class IRValue;

class IRPHINode;

class IRBinaryOperator;

class LoopInfo;

class BasicInductionVariable {
private:
    IRValue *incrementVal{};
    IRValue *baseVal{};
    IRPHINode *phiNode{};
    IRBinaryOperator *calcNode{};
    LoopInfo *belongsTo{};
public:
    BasicInductionVariable(IRPHINode *phi, IRBinaryOperator *BO, LoopInfo *loopInfo);

    IRValue *getIncrementVal() const;

    IRPHINode *getPhiNode() const;

    IRBinaryOperator *getCalcNode() const;

    LoopInfo *getBelongsTo() const;

    IRValue *getBaseVal() const;
};


#endif //COMPILER_BASICINDUCTIONVARIABLE_H
