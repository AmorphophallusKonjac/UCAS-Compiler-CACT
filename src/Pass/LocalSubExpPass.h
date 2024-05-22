#ifndef COMPILER_LOCALSUBEXPPASS_H
#define COMPILER_LOCALSUBEXPPASS_H

#include "Pass.h"

class LocalSubExpPass : public BasicBlockPass {
public:
    explicit LocalSubExpPass(std::string name = "");

    void runOnBasicBlock(IRBasicBlock &BB) override;
};


#endif //COMPILER_RENAMEPASS_H
