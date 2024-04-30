#include "MemToRegPass.h"
#include "IR/iMemory.h"
#include "utils/DominatorTree.h"
#include "IR/iTerminators.h"

#include <utility>
#include <map>

class DominatorTree;

MemToRegPass::MemToRegPass(std::string name) : FunctionPass(std::move(name)) {

}

void MemToRegPass::runOnFunction(IRFunction &F) {
    std::vector<IRAllocaInst *> Allocs;
    DominatorTree &root = getDominatorTree(&F);
    auto entryBB = F.getEntryBlock();
    while (true) {
        Allocs.clear();
        auto instList = entryBB->getInstList();
        for (auto inst: instList) {
            if (auto AI = dynamic_cast<IRAllocaInst *>(inst)) {
                if (isAllocaPromotable(AI)) {
                    Allocs.push_back(AI);
                }
            }
        }
        if (Allocs.empty())
            break;
        mem2reg(Allocs, &root, F.getBasicBlockList());
    }
}

bool MemToRegPass::isAllocaPromotable(IRAllocaInst *AI) {
    return AI->getAllocatedType()->isPrimitiveType();
}

DominatorTree &MemToRegPass::getDominatorTree(IRFunction *F) {
    auto root = F->getEntryBlock()->getNode();
    std::vector<DominatorTree *> vertex;
    dfs(nullptr, &root, vertex);
    for (int i = vertex.size() - 2; i >= 0; i = i - 1) {
        auto node = vertex[i];
        auto parent = node->parent;
        auto s = parent;
        for (auto use: node->basicBlock->getUses()) {
            auto v = &dynamic_cast<IRTerminatorInst *>(use->getUser())->getParent()->getNode();
            DominatorTree *newS;
            if (v->dfnum <= node->dfnum) {
                newS = v;
            } else {
                newS = ancestorWithLowestSemi(v)->semi;
            }
            if (newS->dfnum < s->dfnum) {
                s = newS;
            }
        }
        node->semi = s;
        s->bucket.insert(node);
        link(parent, node);
        for (auto v: parent->bucket) {
            auto y = ancestorWithLowestSemi(v);
            if (y->semi == v->semi) {
                v->idom = parent;
                v->idom->children.push_back(v);
            } else {
                v->samedom = y;
            }
        }
        parent->bucket.clear();
    }
    for (auto n: vertex) {
        if (n->samedom) {
            n->idom = n->samedom->idom;
            n->idom->children.push_back(n);
        }
    }
    computeDominanceFrontier(&F->getEntryBlock()->getNode());
    return F->getEntryBlock()->getNode();
}

void MemToRegPass::dfs(DominatorTree *p, DominatorTree *n, std::vector<DominatorTree *> &vertex) {
    assert(n && "wtf");
    if (n->dfnum == 0) {
        if (p)
            n->dfnum = p->dfnum + 1;
        else
            n->dfnum = 1;
        vertex.push_back(n);
        n->parent = p;
        auto BB = n->basicBlock;
        assert(BB->hasTerminator() && "BB has no terminator");
        auto terminator = BB->getTerminator();
        for (unsigned i = 0, e = terminator->getNumSuccessors(); i != e; ++i) {
            dfs(n, &terminator->getSuccessor(i)->getNode(), vertex);
        }
    }
}

void MemToRegPass::link(DominatorTree *p, DominatorTree *n) {
    assert(n && "wtf");
    n->ancestor = p;
}

DominatorTree *MemToRegPass::ancestorWithLowestSemi(DominatorTree *v) {
    DominatorTree *u = v;
    while (v->ancestor) {
        if (v->semi->dfnum < u->semi->dfnum) {
            u = v;
        }
        v = v->ancestor;
    }
    return u;
}

void
MemToRegPass::mem2reg(std::vector<IRAllocaInst *> Allocs, DominatorTree *root, std::vector<IRBasicBlock *> &BBList) {
    std::set<IRBasicBlock *> defineBlock;
    std::set<IRBasicBlock *> userBlock;
    /*!
     * 首先遍历每个块：
     * 1. 将不会跨基本块传值的store删去。
     * 2. 将使用本块中store值的load删去。
     */
    for (auto BB: BBList) {
        auto &instList = BB->getInstList();
        std::vector<IRInstruction *> bin;
        bin.clear();
        std::map<IRValue *, IRStoreInst *> lastStore;
        lastStore.clear();
        for (auto inst: instList) {
            if (auto loadInst = dynamic_cast<IRLoadInst *>(inst)) {
                //! 对于变量的load其pointer一定是allca指令
                if (!dynamic_cast<IRAllocaInst *>(loadInst->getPointerOperand())) continue;

                if (auto lastStoreInst = lastStore[loadInst->getPointerOperand()]) {
                    loadInst->replaceAllUsesWith(lastStoreInst->getOperand(0));
                    bin.push_back(dynamic_cast<IRInstruction *>(loadInst));
                }
            } else if (auto storeInst = dynamic_cast<IRStoreInst *>(inst)) {
                //! 对于变量的store其1pointer一定是allca指令
                if (!dynamic_cast<IRAllocaInst *>(storeInst->getPointerOperand())) continue;

                if (auto inst = lastStore[storeInst->getPointerOperand()]) {
                    bin.push_back(dynamic_cast<IRInstruction *>(inst));
                }
                lastStore[storeInst->getPointerOperand()] = storeInst;
            }
        }
        for (auto trash: bin) {
            instList.erase(std::find(instList.begin(), instList.end(), trash));
        }
    }
    for (auto alloc: Allocs) {
        auto userList = dynamic_cast<IRValue *>(alloc)->getUses();
    }
}

void MemToRegPass::computeDominanceFrontier(DominatorTree *node) {
    std::set<DominatorTree *> S;
    S.clear();
    auto succList = node->basicBlock->getTerminator();
    for (unsigned i = 0, e = succList->getNumSuccessors(); i != e; ++i) {
        auto succ = &succList->getSuccessor(i)->getNode();
        if (succ->idom != node) {
            S.insert(succ);
        }
    }
    for (auto ch: node->children) {
        computeDominanceFrontier(ch);
        for (auto w: ch->DF) {
            if (node == w || !isAncestor(node, w)) {
                S.insert(w);
            }
        }
    }
    node->DF = S;
}

bool MemToRegPass::isAncestor(DominatorTree *p, DominatorTree *ch) {
    for (auto n = ch; n; n = n->idom) {
        if (n == p) {
            return true;
        }
    }
    return false;
}
