#include "MemToRegPass.h"
#include "IR/iMemory.h"
#include "utils/DominatorTree.h"
#include "IR/iTerminators.h"
#include "IR/iPHINdoe.h"

#include <utility>
#include <map>
#include <stack>

class DominatorTree;

MemToRegPass::MemToRegPass(std::string name) : FunctionPass(std::move(name)) {

}

void MemToRegPass::runOnFunction(IRFunction &F) {
    std::vector<IRAllocaInst *> Allocs;
    DominatorTree *root = getDominatorTree(&F);
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
        mem2reg(Allocs, root, F.getBasicBlockList());
    }
}

bool MemToRegPass::isAllocaPromotable(IRAllocaInst *AI) {
    return AI->getAllocatedType()->isPrimitiveType();
}

DominatorTree *MemToRegPass::getDominatorTree(IRFunction *F) {
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

void MemToRegPass::dfs(DominatorTree *p, DominatorTree *n, std::vector<DominatorTree *> &vertex) {
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
    std::set<DominatorTree *> defineNode;
    std::set<DominatorTree *> userNode;
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
                //! 对于变量的store其pointer一定是allca指令
                if (!dynamic_cast<IRAllocaInst *>(storeInst->getPointerOperand())) continue;

                if (auto lasInst = lastStore[storeInst->getPointerOperand()]) {
                    bin.push_back(dynamic_cast<IRInstruction *>(lasInst));
                }
                lastStore[storeInst->getPointerOperand()] = storeInst;
            }
        }
        for (auto trash: bin) {
            trash->dropAllReferences();
            instList.erase(std::find(instList.begin(), instList.end(), trash));
        }
    }
    /*!
     * 开始对每个变量开始优化
     */
    for (auto alloc: Allocs) {
        /*!
         * 计算 defineBLock 和 userBlock 还有 orig
         */
        defineNode.clear();
        userNode.clear();
        auto userList = dynamic_cast<IRValue *>(alloc)->getUses();
        for (auto use: userList) {
            auto useInst = dynamic_cast<IRInstruction *>(use->getUser());
            auto block = useInst->getParent();
            if (IRLoadInst::classof(useInst)) {
                userNode.insert(block->getNode());
            } else if (IRStoreInst::classof(useInst)) {
                defineNode.insert(block->getNode());
                block->getNode()->orig.insert(alloc);
            } else {
                assert(0 && "wtf");
            }
        }

        /*!
         * 特例：对于只赋值一次的变量进行优化
         */
        std::vector<IRInstruction *> bin;
        bin.clear();
        if (defineNode.size() == 1) {
            IRValue *replaceValue = nullptr;
            for (auto use: userList) {
                auto useInst = dynamic_cast<IRInstruction *>(use->getUser());
                if (IRStoreInst::classof(useInst)) {
                    replaceValue = dynamic_cast<IRStoreInst *>(useInst)->getOperand(0);
                    bin.push_back(useInst);
                    break;
                }
            }
            for (auto use: userList) {
                auto useInst = dynamic_cast<IRInstruction *>(use->getUser());
                if (IRLoadInst::classof(useInst)) {
                    useInst->replaceAllUsesWith(replaceValue);
                    bin.push_back(useInst);
                }
            }
        } else {
            /*!
             * 对于其他，按正常ssa转换方法进行优化，插入phi节点
             */
            std::set<DominatorTree *> workList;
            workList = defineNode;
            while (!workList.empty()) {
                auto node = *workList.begin();
                workList.erase(workList.begin());
                for (auto DFNode: node->DF) {
                    if (DFNode->phi.find(alloc) == DFNode->phi.end()) {
                        DFNode->phi.insert(alloc);
                        auto phi = new IRPHINode(const_cast<IRType *>(alloc->getAllocatedType()), alloc);
                        DFNode->basicBlock->addInstructionToFront(dynamic_cast<IRInstruction *>(phi));
                        if (node->orig.find(alloc) == node->phi.end()) {
                            workList.insert(DFNode);
                        }
                    }
                }
            }
            /*!
             * 重命名轮，删除load和store，填入phi
             */
            std::stack<IRValue *> valueStack;
            while (!valueStack.empty()) valueStack.pop();
            renamePass(root, alloc, &bin, &valueStack);
        }
        for (auto trash: bin) {
            auto &list = trash->getParent()->getInstList();
            trash->dropAllReferences();
            list.erase(std::find(list.begin(), list.end(), trash));
        }
        auto &list = alloc->getParent()->getInstList();
        dynamic_cast<IRInstruction *>(alloc)->dropAllReferences();
        list.erase(std::find(list.begin(), list.end(), alloc));
    }

}

void MemToRegPass::computeDominanceFrontier(DominatorTree *node) {
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

bool MemToRegPass::isAncestor(DominatorTree *p, DominatorTree *ch) {
    for (auto n = ch; n; n = n->idom) {
        if (n == p) {
            return true;
        }
    }
    return false;
}

void MemToRegPass::renamePass(DominatorTree *node, IRAllocaInst *alloc, std::vector<IRInstruction *> *bin,
                              std::stack<IRValue *> *valueStack) {
    int cnt = 0;
    auto &instList = node->basicBlock->getInstList();
    for (auto inst: instList) {
        if (IRLoadInst::classof(inst)) {
            if (dynamic_cast<IRLoadInst *>(inst)->getPointerOperand() == dynamic_cast<IRValue *>(alloc)) {
                inst->replaceAllUsesWith(valueStack->top());
                bin->push_back(inst);
            }
        } else if (IRStoreInst::classof(inst)) {
            if (dynamic_cast<IRStoreInst *>(inst)->getPointerOperand() == dynamic_cast<IRValue *>(alloc)) {
                ++cnt;
                valueStack->push(inst->getOperand(0));
                bin->push_back(inst);
            }
        } else if (IRPHINode::classof(inst)) {
            if (dynamic_cast<IRPHINode *>(inst)->getVar() == alloc) {
                ++cnt;
                valueStack->push(dynamic_cast<IRValue *>(inst));
            }
        }
    }
    auto terminator = node->basicBlock->getTerminator();
    unsigned numSuccessors = terminator->getNumSuccessors();
    for (int i = 0; i < numSuccessors; ++i) {
        auto BB = terminator->getSuccessor(i);
        auto &instList = BB->getInstList();
        IRPHINode *phi = nullptr;
        for (auto inst: instList) {
            if (IRPHINode::classof(inst) && dynamic_cast<IRPHINode *>(inst)->getVar() == alloc) {
                phi = dynamic_cast<IRPHINode *>(inst);
                break;
            }
            if (!IRPHINode::classof(inst))
                break;
        }
        if (phi) {
            phi->addIncoming(valueStack->top(), node->basicBlock);
        }
    }
    for (auto child: node->children) {
        renamePass(child, alloc, bin, valueStack);
    }
    for (int i = 0; i < cnt; ++i) {
        valueStack->pop();
    }
}
