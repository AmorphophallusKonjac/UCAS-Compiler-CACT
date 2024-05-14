#ifndef COMPILER_STRENGTHREDUCTIONPASS_H
#define COMPILER_STRENGTHREDUCTIONPASS_H

#include "Pass.h"
#include "utils/LoopInfo.h"
#include "utils/BasicInductionVariable.h"

class StrengthReductionPass : public FunctionPass {
public:
    explicit StrengthReductionPass(std::string name = "");

    void runOnFunction(IRFunction &F) override;

private:
    static std::set<BasicInductionVariable *> findBasicInductionVar(LoopInfo *loop);

    static bool isLoopInvariant(IRValue *value, LoopInfo *loop);

    static void reduction(IRFunction *F, BasicInductionVariable *const &BI, LoopInfo *loop,
                          std::set<BasicInductionVariable *> *BISet);

    static IRBinaryOperator *findInductionVar(BasicInductionVariable *const &BI, LoopInfo *loop);
};


#endif //COMPILER_STRENGTHREDUCTIONPASS_H
