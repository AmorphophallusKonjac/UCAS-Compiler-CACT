#include "Optimizer.h"
#include "Pass/RenamePass.h"
#include "Pass/CutDeadBlockPass.h"
#include "Pass/MemToRegPass.h"
#include "Pass/AlgebraicPass.h"
#include "Pass/ConstantPass.h"
#include "Pass/LocalSubExpPass.h"
#include "Pass/HoistingLoopInvariantValuePass.h"
#include "Pass/StrengthReductionPass.h"
#include "Pass/GlobalSubExpPass.h"
#include "Pass/LoopArrayScalarizePass.h"
#include "Pass/CutDeadCodePass.h"
#include "Pass/EliminateBasicInductionVarPass.h"
#include "Pass/RegisterPass.h"
#include "Pass/AggressiveDeadCodeEliminatePass.h"
#include "Pass/PHIdeletePass.h"

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
    addPass(new MemToRegPass("Mem2Reg"));

    addPass(new HoistingLoopInvariantValuePass("HoistingLoopInvariantValue"));
    addPass(new LocalSubExpPass("LocalSubExpPass"));
    addPass(new GlobalSubExpPass("GlobalSubExpPass"));
    addPass(new ConstantPass("ConstantPass"));

    addPass(new AggressiveDeadCodeEliminatePass("AggressiveDeadCodeEliminate"));

    //addPass(new CutDeadBlockPass("CutDeadBLock"));

    addPass(new RenamePass("RenamePass"));
    
//    addPass(new CutDeadBlockPass("CutDeadBLock"));


    //addPass(new PHIdeletePass("PHIdeletePass"));
    // addPass(new RegisterPass("RegisterPass"));
}
