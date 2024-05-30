#ifndef COMPILER_PHIDELETEPASS_H
#define COMPILER_PHIDELETEPASS_H

#include "Pass.h"
#include "utils/Register.h"

class PHIdeletePass : public FunctionPass {
public:
    explicit PHIdeletePass(std::string name = "");

    void runOnFunction(IRFunction &F) override;
};

#endif //COMPILER_PHIDELETEPASS_H
