#include "RegisterNode.h"
#include "IR/IRInstruction.h"
#include "IR/IRValue.h"
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
                dynamic_cast<IRInstruction*>(inst->getOperand(0))->getRegNode()->moveList.insert(inst);
                dynamic_cast<IRInstruction*>(inst->getOperand(1))->getRegNode()->moveList.insert(inst);

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

    /*构建unode,vnode*/
    RegisterNode* unode;
    RegisterNode* vnode;
    if(std::find(precolored.begin(), precolored.end(), src) != precolored.end()){ unode = src; vnode = dst; }
    else{ unode = dst; vnode = src; }

    worklistMoves.erase(worklistMoves.begin());
    if(unode == vnode){                                                                                                 //两个node一样
        coalescedMoves.push_back(move);
        AddWorkList(unode);
    }else if(std::find(precolored.begin(), precolored.end(), vnode) != precolored.end() && 
             std::find(adjSet.begin(), adjSet.end(), std::make_tuple(unode, vnode)) != adjSet.end()){   //vnode,unode都是机器寄存器
                constrainedMoves.push_back(move);
                AddWorkList(unode);
                AddWorkList(vnode);
    }else if((std::find(precolored.begin(), precolored.end(), unode) != precolored.end() && George(vnode, unode)) ||    //采用George保守合并策略
             (std::find(precolored.begin(), precolored.end(), unode) == precolored.end() && Briggs(vnode, unode))       //采用Briggs保守合并策略
                ){                                                                                                        
                    coalescedMoves.push_back(move);
                    Combine(unode, vnode);
                    AddWorkList(unode);
    }else{
        activeMoves.push_back(move);
    }
}

/*返回alias*/
RegisterNode* RegisterNode::GetAlias(RegisterNode* node){
    if(std::find(coalescedNodes.begin(), coalescedNodes.end(), node) != coalescedNodes.end()){
        return GetAlias(alias[node]);
    }else{
        return node;
    }
}

/*即满足低度数传送无关的节点，可以加入simplifyWorkList*/
void RegisterNode::AddWorkList(RegisterNode* node){
    if( std::find(precolored.begin(), precolored.end(), node) == precolored.end() &&
        !MoveRelated(node) && (degree[node]<regNum)){
            freezeWorklist.erase(std::find(freezeWorklist.begin(), freezeWorklist.end(), node));
            simplifyWorklist.push_back(node);
        }
}

/*考虑到tnode是机器寄存器，t与v邻接或tnode可以合并*/
bool RegisterNode::OK(RegisterNode* tnode, RegisterNode* rnode){
    return  degree[tnode]<regNum ||
            std::find(precolored.begin(), precolored.end(), tnode)!=precolored.end() ||
            std::find(adjSet.begin(), adjSet.end(), std::make_tuple(tnode, rnode))!=adjSet.end();
}

/*临邻接高度数结点不能大于K*/
bool RegisterNode::Conservative(std::vector<RegisterNode*> nodes){
    unsigned k=0;
    for(auto nnode: nodes){
        if(degree[nnode]>=regNum)
            k++;
    }
    return (k<regNum);
}

bool RegisterNode::Briggs(RegisterNode* vnode, RegisterNode* unode){
    auto uadj = Adjcent(unode);
    auto vadj = Adjcent(vnode);
    std::vector<RegisterNode*> uvunion(uadj.size() + vadj.size());

    /*unadj是一个迭代器*/
    auto uvadj = std::set_union(uadj.begin(), uadj.end(), vadj.begin(), vadj.end(), uvunion.begin());
    uvunion.resize(uvadj - uvunion.begin());
    return Conservative(uvunion);
}

bool RegisterNode::George(RegisterNode* vnode, RegisterNode* unode){
    bool flag = true;
    auto vadj = Adjcent(vnode);
    for(auto tnode: vadj){
        flag &= OK(tnode, unode);
    }
    return flag;
}

void RegisterNode::Combine(RegisterNode* unode, RegisterNode* vnode){
    /*将vnode从哪个工作集中删除*/
    if(std::find(freezeWorklist.begin(), freezeWorklist.end(), vnode) != freezeWorklist.end()){
        freezeWorklist.erase(std::find(freezeWorklist.begin(), freezeWorklist.end(), vnode));
    }else{
        spillWorklist.erase(std::find(spillWorklist.begin(), spillWorklist.end(), vnode));
    }
    coalescedNodes.push_back(vnode);

    /*unode传送有关的指令给到vnode,vnode进行enable*/
    alias[vnode]=unode;
    for(auto move: vnode->moveList){
        unode->moveList.insert(move);
    }
    EnableMoves({vnode});

    /*vnode的邻接结点处理到unode上*/
    for(auto tnode:Adjcent(vnode)){
        AddEdge(tnode, vnode);
        DecrementDegree(tnode);
    }

    /*经过处理之后，unode可能需要加入到溢出工作集中(如果>=K)*/
    if(degree[unode]>=regNum && std::find(freezeWorklist.begin(), freezeWorklist.end(), unode)!=freezeWorklist.end()){
        freezeWorklist.erase(std::find(freezeWorklist.begin(), freezeWorklist.end(), unode));
        spillWorklist.push_back(unode);
    }
}

void RegisterNode::Freeze(){
    RegisterNode* unode = *freezeWorklist.begin();

    /*首先unode变成可简化的节点==结点，然后对与其相关的move指令的其他节点进行冻结操作*/
    freezeWorklist.erase(freezeWorklist.begin());
    simplifyWorklist.push_back(unode);
    FreezeMoves(unode);
}

/*对应的unode在被冻结之后，考虑unode相关的传送指令对应的其他结点是否要加入简化工作集中*/
void RegisterNode::FreezeMoves(RegisterNode* unode){

    RegisterNode* xnode;
    RegisterNode* ynode;
    RegisterNode* vnode;
    for(auto move: NodeMoves(unode)){
        xnode = dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(move)->getDest())->getRegNode();
        ynode = dynamic_cast<IRInstruction*>(dynamic_cast<IRMoveInst*>(move)->getSrc())->getRegNode();

        /*通过检查最终将vnode,unode表示为该move指令对应的两个node*/
        if(GetAlias(ynode) == GetAlias(unode)){
            vnode = GetAlias(xnode);
        }else{
            vnode = GetAlias(ynode);
        }

        /*这条传送指令将被冻结*/
        activeMoves.erase(std::find(activeMoves.begin(), activeMoves.end(), move));
        frozenMoves.push_back(move);

        /*低度数传送有关结点变成低度数传送无关结点*/
        if(NodeMoves(vnode).empty() && degree[vnode]<regNum){
            freezeWorklist.erase(std::find(freezeWorklist.begin(), freezeWorklist.end(), vnode));
            simplifyWorklist.push_back(vnode);
        }
    }                                                   
}

void RegisterNode::SelectSpill(){
    RegisterNode* mnode = *spillWorklist.begin();

    spillWorklist.erase(spillWorklist.begin());
    simplifyWorklist.push_back(mnode);
    FreezeMoves(mnode);
}

void RegisterNode::AssignColors(){
    while(!selectStack.empty()){
        RegisterNode* nnode = selectStack.back();
        selectStack.pop_back();
        
        /*初始化颜色种类*/
        std::vector<unsigned> okColors;
        for(unsigned i=0; i<regNum; i++){
            okColors.push_back(i);
        }

        /*若邻接结点已被染色，则选择数目减少*/
        for(auto wnode: nnode->adjList){
            if( std::find(coloredNodes.begin(), coloredNodes.end(), GetAlias(wnode)) != coloredNodes.end() ||
                std::find(precolored.begin(), precolored.end(), GetAlias(wnode)) != precolored.end()){
                    okColors.erase(std::find(okColors.begin(), okColors.end(), GetAlias(wnode)->color));
                }
        }

        if(okColors.empty()){
            spilledNodes.push_back(nnode);
        }else{
            coloredNodes.push_back(nnode);
            /*这里可以考虑优先选择那个寄存器进行染色*/
                                                                                                                                                                                                        nnode->color = *okColors.begin();
        }

        okColors.clear();
    }

    /*已经被合并的结点采用与其合并的寄存器结点的颜色*/
    for(auto nnode: coalescedNodes)
        nnode->color = GetAlias(nnode)->color;
}