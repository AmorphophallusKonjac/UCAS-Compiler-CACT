#include "DominatorTree.h"
#include "IR/IRBasicBlock.h"

DominatorTree::DominatorTree(IRBasicBlock *BB) : basicBlock(BB), bucket(), children(), DF(), orig(), phi() {

}
