#ifndef COMPILER_CUTDEADCODEPASS_H
#define COMPILER_CUTDEADCODEPASS_H

#include "Pass.h"

class CutDeadCodePass : public BasicBlockPass {
public:
    explicit CutDeadCodePass(std::string name);

    void runOnBasicBlock(IRBasicBlock &BB) override;
};


#endif //COMPILER_CUTDEADCODEPASS_H
