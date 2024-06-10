#ifndef COMPILER_DELETENONEPREDSPASS_H
#define COMPILER_DELETENONEPREDSPASS_H

#include "Pass.h"

class DeleteNonePredsPass : public FunctionPass {
public:
    explicit DeleteNonePredsPass(std::string name = "", int level = 0);

    void runOnFunction(IRFunction &F) override;
};

#endif //COMPILER_DELETEPREDSNONEPASS_H
