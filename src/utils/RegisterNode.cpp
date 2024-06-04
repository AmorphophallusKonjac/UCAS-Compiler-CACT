#include "RegisterNode.h"
#include "IR/IRvalue.h"
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
std::map<std::string, unsigned> RegisterNode::degree {};
std::map<RegisterNode*, RegisterNode*> RegisterNode::alias {};    

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
            degree[u->RegisterNodeName]++;
        }

        if(std::find(precolored.begin(), precolored.end(), v) == precolored.end()){
            v->adjList.push_back(u);
            degree[v->RegisterNodeName]++;
        }
    }
}

void RegisterNode::MakeWorklist(){
    for(auto node: initial){
        initial.erase(std::find(initial.begin(), initial.end(), node));

        if(degree[node->RegisterNodeName] >= GPRNUM){}
    }
}