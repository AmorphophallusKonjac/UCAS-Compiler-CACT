#ifndef COMPILER_DOMINATORTREE_H
#define COMPILER_DOMINATORTREE_H

#include <set>
#include <vector>

class IRBasicBlock;

class IRAllocaInst;

class IRFunction;

class DominatorTree {
public:
    explicit DominatorTree(IRBasicBlock *BB);

    ~DominatorTree() = default;

    static DominatorTree *getDominatorTree(IRFunction *F);

    static bool isAncestor(DominatorTree *p, DominatorTree *ch);

    static void printDominatorTree(IRFunction *F);

    int dfnum = 0;
    DominatorTree *parent = nullptr;
    DominatorTree *semi = nullptr;
    DominatorTree *idom = nullptr; // 本节点在支配树上的父亲
    DominatorTree *samedom = nullptr;
    DominatorTree *ancestor = nullptr;
    IRBasicBlock *basicBlock;
    std::set<DominatorTree *> bucket;
    std::vector<DominatorTree *> children; // 本节点在支配树的孩子
    std::set<DominatorTree *> DF; // 本节点的支配边界
    std::set<IRAllocaInst *> orig; // 在本节点定义的所有变量的集合
    std::set<IRAllocaInst *> phi; // 在本节点需要有phi的集合
private:

    static void dfs(DominatorTree *p, DominatorTree *n, std::vector<DominatorTree *> &vertex);

    static void link(DominatorTree *p, DominatorTree *n);

    static DominatorTree *ancestorWithLowestSemi(DominatorTree *v);

    static void computeDominanceFrontier(DominatorTree *node);

    static void resetNode(DominatorTree *n);
};


#endif //COMPILER_DOMINATORTREE_H
