#ifndef COMPILER_DOMINATORTREE_H
#define COMPILER_DOMINATORTREE_H

#include "IR/IRBasicBlock.h"
#include <set>

class IRBasicBlock;

class DominatorTree {
public:
    explicit DominatorTree(IRBasicBlock *BB);

    ~DominatorTree() = default;

    int dfnum = 0;
    DominatorTree *parent = nullptr;
    DominatorTree *semi = nullptr;
    DominatorTree *idom = nullptr;
    DominatorTree *samedom = nullptr;
    DominatorTree *ancestor = nullptr;
    IRBasicBlock *basicBlock;
    std::set<DominatorTree *> bucket;
    std::vector<DominatorTree *> children;
    std::set<DominatorTree *> DF;
};


#endif //COMPILER_DOMINATORTREE_H
