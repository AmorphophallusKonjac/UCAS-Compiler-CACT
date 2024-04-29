#ifndef COMPILER_OPTIMIZER_H
#define COMPILER_OPTIMIZER_H

#include <vector>
#include "Pass/Pass.h"

class Pass;

class Optimizer {
public:
    explicit Optimizer(IRModule *ir);
    void run();
    void addPass(Pass * pass);
    void build();
private:
    std::vector<Pass *> passList;
    IRModule *ir;
};


#endif //COMPILER_OPTIMIZER_H
