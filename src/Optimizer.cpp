#include "Optimizer.h"
#include "Pass/RenamePass.h"
#include "Pass/CutDeadBlockPass.h"
#include "Pass/MemToRegPass.h"
#include "Pass/AlgebraicPass.h"
#include "Pass/ConstantPass.h"

void Optimizer::run() {
    for (auto pass: passList) {
        pass->run(*ir);
    }
}

Optimizer::Optimizer(IRModule *ir)
        : ir(ir), passList() {

}

void Optimizer::addPass(Pass *pass) {
    passList.push_back(pass);
}

void Optimizer::build() {
    addPass(new CutDeadBlockPass("CutDeadBlockPass0"));
    addPass(new MemToRegPass("Mem2Reg"));
//    addPass(new CutDeadBlockPass("CutDeadBlockPass1"));
    addPass(new ConstantPass("ConstantPass"));
    addPass(new AlgebraicPass("AlgebraicPass"));
    addPass(new RenamePass("RenamePass"));
}
