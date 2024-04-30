#ifndef COMPILER_MEMTOREGPASS_H
#define COMPILER_MEMTOREGPASS_H

#include "Pass.h"
#include "IR/iMemory.h"
#include "utils/DominatorTree.h"

class DominatorTree;

class MemToRegPass : public FunctionPass {
public:
    explicit MemToRegPass(std::string name = "");

    void runOnFunction(IRFunction &F) override;

private:
    static bool isAllocaPromotable(IRAllocaInst *AI);

    static DominatorTree &getDominatorTree(IRFunction *F);

    static void dfs(DominatorTree *p, DominatorTree *n, std::vector<DominatorTree *> &vertex);

    static void link(DominatorTree *p, DominatorTree *n);

    static DominatorTree *ancestorWithLowestSemi(DominatorTree *v);

    static void mem2reg(std::vector<IRAllocaInst *> Allocs, DominatorTree *root, std::vector<IRBasicBlock *> &BBList);

    static void computeDominanceFrontier(DominatorTree *node);

    static bool isAncestor(DominatorTree *pTree, DominatorTree *pTree1);

};


#endif //COMPILER_MEMTOREGPASS_H
