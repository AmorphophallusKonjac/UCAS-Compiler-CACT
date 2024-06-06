#ifndef COMPILER_A0MOVEPASS_H
#define COMPILER_A0MOVEPASS_H

#include "Pass.h"

class A0MovePass : public FunctionPass {
public:
    explicit A0MovePass(std::string name = "", int level = 0);

    void runOnFunction(IRFunction &F) override;
};


#endif //COMPILER_A0MOVEPASS_H
