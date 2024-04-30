#ifndef COMPILER_DOMINATORTREE_H
#define COMPILER_DOMINATORTREE_H

#include <set>
#include <vector>

class IRBasicBlock;

class IRAllocaInst;

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
    std::set<IRAllocaInst *> orig; // 在本节点定义的所有变量的集合
    std::set<IRAllocaInst *> phi; // 在本节点需要有phi的集合
};


#endif //COMPILER_DOMINATORTREE_H
