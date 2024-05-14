#include "BasicInductionVariable.h"

#include "IR/iOperators.h"
#include "IR/iPHINdoe.h"

BasicInductionVariable::BasicInductionVariable(IRPHINode *phi, IRBinaryOperator *BO, LoopInfo *loopInfo)
        : phiNode(phi), calcNode(BO), belongsTo(loopInfo) {
    if (BO->getOperand(0) == dynamic_cast<IRValue *>(phi))
        incrementVal = calcNode->getOperand(1);
    else
        incrementVal = calcNode->getOperand(0);
    for (unsigned i = 0, E = phi->getNumIncomingValues(); i < E; ++i) {
        if (phi->getIncomingValue(i) == dynamic_cast<IRValue *>(BO))
            continue;
        else {
            baseVal = phi->getIncomingValue(i);
            break;
        }
    }
}

IRValue *BasicInductionVariable::getIncrementVal() const {
    return incrementVal;
}

IRPHINode *BasicInductionVariable::getPhiNode() const {
    return phiNode;
}

IRBinaryOperator *BasicInductionVariable::getCalcNode() const {
    return calcNode;
}

LoopInfo *BasicInductionVariable::getBelongsTo() const {
    return belongsTo;
}

IRValue *BasicInductionVariable::getBaseVal() const {
    return baseVal;
}
