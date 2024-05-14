#include "StrengthReductionPass.h"

#include <utility>
#include "utils/LoopInfo.h"
#include "IR/iPHINdoe.h"
#include "IR/iOperators.h"
#include "IR/IRConstant.h"
#include <iostream>

StrengthReductionPass::StrengthReductionPass(std::string name) : FunctionPass(std::move(name)) {

}

void StrengthReductionPass::runOnFunction(IRFunction &F) {
    auto loopList = LoopInfo::findLoop(&F);
    for (auto loop: loopList) {
        auto loopBBList = loop->getBasicBlockList();
        auto BISet = findBasicInductionVar(loop);
        while (!BISet.empty()) {
            auto BI = BISet.begin();
            BISet.erase(BI);
            reduction(&F, *BI, loop, &BISet);
        }
    }
}

std::set<BasicInductionVariable *> StrengthReductionPass::findBasicInductionVar(LoopInfo *loop) {
    std::set<BasicInductionVariable *> BISet;
    auto loopBBList = loop->getBasicBlockList();
    auto header = loop->getHeader();
    for (auto inst: header->getInstList()) {
        if (IRPHINode::classof(inst)) {
            auto phi = dynamic_cast<IRPHINode *>(inst);
            for (unsigned i = 0, E = phi->getNumIncomingValues(); i < E; ++i) {
                auto val = phi->getIncomingValue(i);
                auto BB = phi->getIncomingBlock(i);

                if (std::find(loopBBList.begin(), loopBBList.end(), BB) == loopBBList.end()) // 上一个块得是循环内的块
                    continue;

                auto BO = dynamic_cast<IRBinaryOperator *>(val);

                if (BO == nullptr) // 值必须是指令
                    continue;

                if (BO->getOpcode() != IRInstruction::Add) // 是加法
                    continue;

                if (std::find(loopBBList.begin(), loopBBList.end(), BO->getParent()) == loopBBList.end()) // 加法在循环内
                    continue;

                if (BO->getOperand(0) == dynamic_cast<IRValue *>(phi) && isLoopInvariant(BO->getOperand(1), loop) ||
                    BO->getOperand(1) == dynamic_cast<IRValue *>(phi) && isLoopInvariant(BO->getOperand(0), loop)) {
                    BISet.insert(new BasicInductionVariable(phi, BO, loop));
                }
            }
        } else {
            break;
        }
    }
    return BISet;
}

bool StrengthReductionPass::isLoopInvariant(IRValue *value, LoopInfo *loop) {
    auto loopBBList = loop->getBasicBlockList();
    if (IRConstant::classof(value) || IRArgument::classof(value)) {
        return true;
    }
    auto inst = dynamic_cast<IRInstruction *>(value);
    if (std::find(loopBBList.begin(), loopBBList.end(), inst->getParent()) == loopBBList.end()) {
        return true;
    }
    return false;
}

void StrengthReductionPass::reduction(IRFunction *F, BasicInductionVariable *const &BI, LoopInfo *loop,
                                      std::set<BasicInductionVariable *> *BISet) {
    IRBinaryOperator *IV = nullptr;
    auto &preheaderInstList = loop->getPreHeader()->getInstList();
    auto &BICalcInstList = BI->getCalcNode()->getParent()->getInstList();
    while (IV = findInductionVar(BI, loop)) {
        auto &IVinstList = IV->getParent()->getInstList();
        IRValue *loopInvariant = nullptr;
        if (IV->getOperand(0) == dynamic_cast<IRValue *>(BI->getPhiNode())) {
            loopInvariant = IV->getOperand(1);
        } else {
            loopInvariant = IV->getOperand(0);
        }
        IRBinaryOperator *newBaseVal = nullptr, *newBI = nullptr, *newIncrementVal = nullptr;
        IRPHINode *newPhi = nullptr;
        switch (IV->getOpcode()) {
            case IRInstruction::Add:
                // 在 preheader 中插入初始值的计算
                newBaseVal = IRBinaryOperator::create(IRInstruction::Add, loopInvariant, BI->getBaseVal(),
                                                      std::to_string(F->getCount()));
                F->addCount();
                newBaseVal->setParent(loop->getPreHeader());
                preheaderInstList.insert(preheaderInstList.end() - 1, newBaseVal);
                // 在 header 中插入 phi
                newPhi = new IRPHINode(newBaseVal->getType(), nullptr, std::to_string(F->getCount()));
                F->addCount();
                newPhi->setParent(loop->getHeader());
                loop->getHeader()->addInstructionToFront(dynamic_cast<IRInstruction *>(newPhi));

                // 在 BI->getCalcNode 下方插入新的 BI
                newBI = IRBinaryOperator::create(IRInstruction::Add, dynamic_cast<IRValue *>(newPhi),
                                                 BI->getIncrementVal(), std::to_string(F->getCount()));
                F->addCount();
                newBI->setParent(BI->getCalcNode()->getParent());
                BICalcInstList.insert(
                        std::find(BICalcInstList.begin(), BICalcInstList.end(),
                                  dynamic_cast<IRInstruction *>(BI->getCalcNode())) + 1,
                        dynamic_cast<IRInstruction *>(newBI));
                // 将 IV 替换成 newPhi
                IV->replaceAllUsesWith(dynamic_cast<IRValue *>(newPhi));
                IV->dropAllReferences();
                IVinstList.erase(std::find(IVinstList.begin(), IVinstList.end(), dynamic_cast<IRInstruction *>(IV)));
                // 填入 newPhi 的参数
                newPhi->addIncoming(dynamic_cast<IRValue *>(newBaseVal), loop->getPreHeader());
                newPhi->addIncoming(dynamic_cast<IRValue *>(newBI), loop->getLatch());
                break;
            case IRInstruction::Mul:
                // 在 preheader 中插入初始值的计算
                newBaseVal = IRBinaryOperator::create(IRInstruction::Mul, loopInvariant, BI->getBaseVal(),
                                                      std::to_string(F->getCount()));
                F->addCount();
                newBaseVal->setParent(loop->getPreHeader());
                preheaderInstList.insert(preheaderInstList.end() - 1, newBaseVal);
                // 在 preheader 中插入增量的计算
                newIncrementVal = IRBinaryOperator::create(IRInstruction::Mul, loopInvariant, BI->getIncrementVal(),
                                                           std::to_string(F->getCount()));
                F->addCount();
                newIncrementVal->setParent(loop->getPreHeader());
                preheaderInstList.insert(preheaderInstList.end() - 1, newIncrementVal);
                // 在 header 中插入 phi
                newPhi = new IRPHINode(newBaseVal->getType(), nullptr, std::to_string(F->getCount()));
                F->addCount();
                newPhi->setParent(loop->getHeader());
                loop->getHeader()->addInstructionToFront(dynamic_cast<IRInstruction *>(newPhi));

                // 在 BI->getCalcNode 下方插入新的 BI
                newBI = IRBinaryOperator::create(IRInstruction::Add, dynamic_cast<IRValue *>(newPhi),
                                                 newIncrementVal, std::to_string(F->getCount()));
                F->addCount();
                newBI->setParent(BI->getCalcNode()->getParent());
                BICalcInstList.insert(
                        std::find(BICalcInstList.begin(), BICalcInstList.end(),
                                  dynamic_cast<IRInstruction *>(BI->getCalcNode())) + 1,
                        dynamic_cast<IRInstruction *>(newBI));
                // 将 IV 替换成 newPhi
                IV->replaceAllUsesWith(dynamic_cast<IRValue *>(newPhi));
                IV->dropAllReferences();
                IVinstList.erase(std::find(IVinstList.begin(), IVinstList.end(), dynamic_cast<IRInstruction *>(IV)));
                // 填入 newPhi 的参数
                newPhi->addIncoming(dynamic_cast<IRValue *>(newBaseVal), loop->getPreHeader());
                newPhi->addIncoming(dynamic_cast<IRValue *>(newBI), loop->getLatch());
                break;
            default:
                assert(0 && "wtf");
        }
        BISet->insert(new BasicInductionVariable(newPhi, newBI, loop));
    }
}

IRBinaryOperator *StrengthReductionPass::findInductionVar(BasicInductionVariable *const &BI, LoopInfo *loop) {
    auto loopBBList = loop->getBasicBlockList();
    for (auto BB: loopBBList) {
        auto instList = BB->getInstList();
        for (auto inst: instList) {
            if (inst == dynamic_cast<IRInstruction *>(BI->getCalcNode()))
                return nullptr;
            if (inst->getOpcode() != IRInstruction::Add && inst->getOpcode() != IRInstruction::Mul)
                continue;
            auto BO = dynamic_cast<IRBinaryOperator *>(inst);
            if (BO->getOperand(0) == dynamic_cast<IRValue *>(BI->getPhiNode()) &&
                isLoopInvariant(BO->getOperand(1), loop)
                ||
                BO->getOperand(1) == dynamic_cast<IRValue *>(BI->getPhiNode()) &&
                isLoopInvariant(BO->getOperand(0), loop)) {
                return BO;
            }
        }
    }
    return nullptr;
}
