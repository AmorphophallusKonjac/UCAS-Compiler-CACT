#include "Optimizer.h"
#include "Pass/RenamePass.h"

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
    addPass(new RenamePass("RenamePass"));
}
