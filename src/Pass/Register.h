#ifndef COMPILER_REGISTER_H
#define COMPILER_REGISTER_H

#include "Pass.h"

class Register : public FunctionPass {
public:
    explicit Register(std::string name = "");

    void runOnFunction(IRFunction &F) override;
};


#endif //COMPILER_REGISTER_H
