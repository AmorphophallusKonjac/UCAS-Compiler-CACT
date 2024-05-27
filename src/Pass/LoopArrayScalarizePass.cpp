#include "LoopArrayScalarizePass.h"
#include "IR/IRFunction.h"
#include "utils/LoopInfo.h"
#include <iostream>
#include <ostream>

void LoopArrayScalarizePass::runOnFunction(IRFunction &F) {
    std::vector<LoopInfo *> loopvector = LoopInfo::findLoop(&F, nullptr);
    for (auto loop: loopvector) {
        std::cout << "PreHeader: " << loop->getPreHeader()->getName() << std::endl;
        std::cout << "Header: " << loop->getHeader()->getName() << std::endl;
        std::cout << "Latch: " << loop->getLatch()->getName() << std::endl;
        std::cout << "Exit: " << loop->getExit()->getName() << std::endl;
        std::cout << "Exiting:";
        for (auto bb: loop->getExiting()) {
            std::cout << " " << bb->getName();
        }
        std::cout << std::endl;
    }
}

LoopArrayScalarizePass::LoopArrayScalarizePass(std::string name) : FunctionPass(name) {

}
