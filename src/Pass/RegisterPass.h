#ifndef COMPILER_REGISTERPASS_H
#define COMPILER_REGISTERPASS_H

#include "Pass.h"
#include "utils/Register.h"

class RegisterPass : public FunctionPass {
public:
    explicit RegisterPass(std::string name = "");

    void runOnFunction(IRFunction &F) override;
};

#endif //COMPILER_REGISTERPASS_H
