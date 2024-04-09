#include "IRArgument.h"

void IRArgument::setParent(IRFunction *parent) {
    Parent = parent;
}

IRArgument::IRArgument(IRType *Ty, const std::string &Name, IRFunction *F)
    : IRValue(Ty, IRValue::ArgumentVal, Name) {
    Parent = nullptr;
    if (F) {
        F->addArgument(this);
    }
}

void IRArgument::print(std::ostream &OS) const {
    // TODO
}
