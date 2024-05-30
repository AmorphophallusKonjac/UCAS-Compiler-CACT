#ifndef COMPILER_LOOPARRAYSCALARIZEPASS_H
#define COMPILER_LOOPARRAYSCALARIZEPASS_H

#include "Pass.h"

class LoopArrayScalarizePass : public FunctionPass {
public:
    explicit LoopArrayScalarizePass(std::string name = "", int level = 2);

    void runOnFunction(IRFunction &F) override;
};


#endif //COMPILER_RENAMEPASS_H
