#include "ControlDependenceGraphVertex.h"
#include <algorithm>
#include <cassert>

ControlDependenceGraphVertex::ControlDependenceGraphVertex(ControlDependenceGraph *P, IRBasicBlock *BB,
                                                           ControlDependenceGraphVertex::vertexType ty)
        : parent(P), basicBlock(BB), ty(ty) {

}

void ControlDependenceGraphVertex::addSuccessor(ControlDependenceGraphVertex *succ) {
    assert(std::find(successors.begin(), successors.end(), succ) == successors.end()
           && "push same edge twice");
    successors.push_back(succ);
    succ->predecessors.push_back(this);
}

const std::vector<ControlDependenceGraphVertex *> &ControlDependenceGraphVertex::getSuccessors() const {
    return successors;
}

const std::vector<ControlDependenceGraphVertex *> &ControlDependenceGraphVertex::getPredecessors() const {
    return predecessors;
}

IRBasicBlock *ControlDependenceGraphVertex::getBasicBlock() const {
    return basicBlock;
}
