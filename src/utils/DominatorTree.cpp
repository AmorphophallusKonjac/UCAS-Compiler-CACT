#include "DominatorTree.h"
#include "IR/IRBasicBlock.h"
#include "IR/IRFunction.h"
#include "IR/iTerminators.h"

DominatorTree::DominatorTree(IRBasicBlock *BB) : basicBlock(BB), bucket(), children(), DF(), orig(), phi() {

}

DominatorTree *DominatorTree::getDominatorTree(IRFunction *F) {
    auto BBList = F->getBasicBlockList();
    for (auto BB : BBList) {
        resetNode(BB->getNode());
    }
    auto root = F->getEntryBlock()->getNode();
    std::vector<DominatorTree *> vertex;
    dfs(nullptr, root, vertex);
    for (unsigned i = vertex.size() - 1; i > 0; i = i - 1) {
        auto node = vertex[i];
        auto parent = node->parent;
        auto s = parent;
        for (auto use: node->basicBlock->getUses()) {
            auto v = dynamic_cast<IRTerminatorInst *>(use->getUser())->getParent()->getNode();
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
    computeDominanceFrontier(F->getEntryBlock()->getNode());
    return F->getEntryBlock()->getNode();
}

void DominatorTree::dfs(DominatorTree *p, DominatorTree *n, std::vector<DominatorTree *> &vertex) {
    assert(n && "wtf");
    if (n->dfnum == 0) {
        vertex.push_back(n);
        n->dfnum = vertex.size();
        n->parent = p;
        auto BB = n->basicBlock;
        assert(BB->hasTerminator() && "BB has no terminator");
        auto terminator = BB->getTerminator();
        for (unsigned i = 0, e = terminator->getNumSuccessors(); i != e; ++i) {
            dfs(n, terminator->getSuccessor(i)->getNode(), vertex);
        }
    }
}

void DominatorTree::link(DominatorTree *p, DominatorTree *n) {
    assert(n && "wtf");
    n->ancestor = p;
}

DominatorTree *DominatorTree::ancestorWithLowestSemi(DominatorTree *v) {
    DominatorTree *u = v;
    while (v->ancestor) {
        if (v->semi->dfnum < u->semi->dfnum) {
            u = v;
        }
        v = v->ancestor;
    }
    return u;
}

void DominatorTree::computeDominanceFrontier(DominatorTree *node) {
 std::set<DominatorTree *> S;
    S.clear();
    auto succList = node->basicBlock->getTerminator();
    for (unsigned i = 0, e = succList->getNumSuccessors(); i != e; ++i) {
        auto succ = succList->getSuccessor(i)->getNode();
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

bool DominatorTree::isAncestor(DominatorTree *p, DominatorTree *ch) {
    for (auto n = ch; n; n = n->idom) {
        if (n == p) {
            return true;
        }
    }
    return false;
}

void DominatorTree::resetNode(DominatorTree *n) {
    n->dfnum = 0;
    n->idom = n->semi = n->samedom = n->ancestor = n->parent = nullptr;
    n->bucket.clear();
    n->children.clear();
    n->DF.clear();
    n->orig.clear();
    n->phi.clear();
}
