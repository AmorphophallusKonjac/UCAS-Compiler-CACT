#include "RegisterNode.h"
#include "IR/IRInstruction.h"
#include "IR/IRvalue.h"
#include "IR/iOther.h"
#include "utils/DominatorTree.h"
#include "utils/Register.h"
#include <algorithm>
#include <vector>
#include <tuple>

unsigned RegisterNode::regNum = 0;

std::list<RegisterNode*> RegisterNode::precolored {};
std::list<RegisterNode*> RegisterNode::initial {};
std::list<RegisterNode*> RegisterNode::simplifyWorklist {};
std::list<RegisterNode*> RegisterNode::freezeWorklist {};
std::list<RegisterNode*> RegisterNode::spillWorklist {};
std::list<RegisterNode*> RegisterNode::spilledNodes {};
std::list<RegisterNode*> RegisterNode::coalescedNodes {};
std::list<RegisterNode*> RegisterNode::coloredNodes {};
std::list<RegisterNode*> RegisterNode::selectStack {};

std::vector<IRInstruction*> RegisterNode::coalescedMoves {};    
std::vector<IRInstruction*> RegisterNode::constrainedMoves {};   
std::vector<IRInstruction*> RegisterNode::frozenMoves {};        
std::vector<IRInstruction*> RegisterNode::worklistMoves {};
std::vector<IRInstruction*> RegisterNode::activeMoves {};                  

std::vector<std::tuple<RegisterNode*, RegisterNode*>> RegisterNode::adjSet {};
std::map<RegisterNode*, unsigned> RegisterNode::degree {};
std::map<RegisterNode*, RegisterNode*> RegisterNode::alias {};    

std::vector<RegisterNode*> RegisterNode::Adjcent(RegisterNode* node){
    std::vector<RegisterNode*> adjcent;

    for(auto adjnode: node->adjList){
        /*如果两处都没有找到，说明既没有被简化入栈也没有被合并，即还在图中，因此它是node当前还有效的邻接结点*/
        if(std::find(selectStack.begin(), selectStack.end(), adjnode) == selectStack.end() &&
           std::find(coalescedNodes.begin(), coalescedNodes.end(), adjnode) == coalescedNodes.end()){
            adjcent.push_back(adjnode);
           }
    }
    return adjcent;
}


/*与这个结点相关的还没有合并的传送指令*/
std::vector<IRInstruction*> RegisterNode::NodeMoves(RegisterNode* node){
    std::vector<IRInstruction*> NodeMoves;
    /*adjList[n]\(selectStack U coalescedNodes)*/
    for(auto irinst: node->moveList){
        if(std::find(activeMoves.begin(), activeMoves.end(), irinst) != activeMoves.end() ||
           std::find(worklistMoves.begin(), worklistMoves.end(), irinst) != worklistMoves.end()){
                NodeMoves.push_back(irinst);
           }
    }
    return NodeMoves;
}

bool RegisterNode::MoveRelated(RegisterNode* node){
    if(!NodeMoves(node).empty()) return true;
    else return false;
};

void RegisterNode::init(WHICH which){
    which = which;
    if(which == GENERAL){ regNum = GPRNUM; }
    else if(which == FLOAT){ regNum = FPRNUM; }
}


void RegisterNode::Build(IRFunction& F){
    for(auto BB: F.getBasicBlockList()){
        for(auto inst: BB->getInstList()){
            if(inst->getOpcode() == IRInstruction::Move){
                /*源操作数与目的操作数均与这条move指令相关*/
                dynamic_cast<IRInstruction*>(inst->getOperand(0))->getRegNode()->moveList.push_back(inst);
                dynamic_cast<IRInstruction*>(inst->getOperand(1))->getRegNode()->moveList.push_back(inst);

                /*有可能合并的传送指令*/
                worklistMoves.push_back(inst);
            }
        }
    }
}

void RegisterNode::AddEdge(RegisterNode* u, RegisterNode* v){
    auto it = std::find(adjSet.begin(), adjSet.end(), std::make_tuple(u,v));

    /*若adjList没有该元素，则加入*/
    if((it==adjSet.end()) && (u!=v)){
        adjSet.push_back(std::make_tuple(u,v));
        adjSet.push_back(std::make_tuple(v,u));

        /*相当于要求不是机器寄存器*/
        if(std::find(precolored.begin(), precolored.end(), u) == precolored.end()){
            u->adjList.push_back(v);
            degree[u]++;
        }

        if(std::find(precolored.begin(), precolored.end(), v) == precolored.end()){
            v->adjList.push_back(u);
            degree[v]++;
        }
    }
}

void RegisterNode::MakeWorklist(){
    for(auto node: initial){
        initial.erase(std::find(initial.begin(), initial.end(), node));

        if(degree[node] >= regNum){
            spillWorklist.push_back(node);          //准备溢出
        }else if(MoveRelated(node)){
            freezeWorklist.push_back(node);         //
        }else{
            simplifyWorklist.push_back(node);       //可以简化该结点
        }
    }
}

/*简化掉低度数结点*/
void RegisterNode::simplify(){
    auto simplifynode = *simplifyWorklist.begin();
    selectStack.push_back(simplifynode);
    simplifyWorklist.erase(simplifyWorklist.begin());

    auto adjcent = Adjcent(simplifynode);
    for(auto adjnode: adjcent){

    }
}

void RegisterNode::DecrementDegree(RegisterNode* node){
    //度数--
    degree[node]--;
    if(degree[node] == regNum-1){
        auto adjmove = Adjcent(node);
        adjmove.push_back(node);
        EnableMoves(adjmove);

        //度数降为低度数，因此不用考虑假溢出，考虑合并或者冻结
        spillWorklist.erase(std::find(spillWorklist.begin(), spillWorklist.end(), node));
        if(MoveRelated(node)){
            freezeWorklist.push_back(node);
        }else{
            simplifyWorklist.push_back(node);
        }
    }
}

/*node中的未做好合并准备的指令加入到准备合并的指令集中*/
void RegisterNode::EnableMoves(std::vector<RegisterNode*> nodes){
    for(auto node: nodes){
        for(auto nodemove: NodeMoves(node)){
            auto movepos = std::find(activeMoves.begin(), activeMoves.end(), nodemove);
            if(movepos != activeMoves.end()){
                activeMoves.erase(movepos);
                worklistMoves.push_back(nodemove);
            }
        }
    }
}

void RegisterNode::Coalesce(){
    auto move = *worklistMoves.begin();
    RegisterNode* dst = GetAlias(dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(move)->getDest())->getRegNode());
    RegisterNode* src = GetAlias(dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(move)->getDest())->getRegNode());
}

/*返回alias*/
RegisterNode* RegisterNode::GetAlias(RegisterNode* node){
    if(std::find(coalescedNodes.begin(), coalescedNodes.end(), node) != coalescedNodes.end()){
        return GetAlias(alias[node]);
    }else{
        return node;
    }
}