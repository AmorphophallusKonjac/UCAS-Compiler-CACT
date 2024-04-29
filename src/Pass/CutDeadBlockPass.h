#ifndef COMPILER_CUTDEADBLOCKPASS_H
#define COMPILER_CUTDEADBLOCKPASS_H

#include "Pass.h"

class CutDeadBlockPass : public FunctionPass {
public:
    explicit CutDeadBlockPass(std::string name = "");

    void runOnFunction(IRFunction &F) override;
};


#endif //COMPILER_CUTDEADBLOCKPASS_H
