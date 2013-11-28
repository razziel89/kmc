/*
 * Copyright 2009-2013 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_KMC_GRAPHBULK_H_
#define __VOTCA_KMC_GRAPHBULK_H_

#include <vector>
#include <votca/kmc/graphsql.h>
//#include <votca/kmc/nodedevice.h>
//#include <votca/kmc/graphcubic.h>

namespace votca { namespace kmc {

enum NodeType {NormalNode, LeftElectrodeNode, RightElectrodeNode};    
    
template<class TGraph, class TNode, class TLink>    
class GraphDevice : public TGraph {

public:

    ///define electrode nodes and form links between those nodes and neighbouring nodes and set maxpairdegree/hopping_distance/sim_box_size
    void Setup_device_graph(double left_distance, double right_distance);       
    
    ///associate all links in links vector to the corresponding nodes
    void LinkSort();
    
    ///break the periodicity of the graph (breaking boundary crossing pairs) .. (run before linksort)
    void Break_periodicity(bool break_x, bool break_y, bool break_z);

    ///calculate the maximum of all degrees in the graph
    int Determine_Max_Pair_Degree();
    
    ///calculate hopping_distance (maximum distance between a pair of nodes) ... needed for injection and coulomb potential calculations
    double Determine_Hopping_Distance();
    
    ///calculate the simulation box size
    votca::tools::vec Determine_Sim_Box_Size();   
    
    /// max_pair_degree
    const int &maxpairdegree() const { return _max_pair_degree; }

    /// hopping distance
    const double &hopdist() const { return _hop_distance; }

    /// simulation box size
    const votca::tools::vec &simboxsize() const { return _sim_box_size; }
    
    /// left electrode node
    TNode* &left() { return _left_electrode; }
    
    /// right electrode node
    TNode* &right() { return _right_electrode; }
    
private:

    void RenumberId();        
    
    int _max_pair_degree;  
    double _hop_distance;
    votca::tools::vec _sim_box_size;

    TNode* _left_electrode;
    TNode* _right_electrode;
    
};

template<class TGraph, class TNode, class TLink>
void GraphDevice<TGraph, TNode, TLink>::Setup_device_graph(double left_distance, double right_distance){

    // Determine hopping distance before breaking periodicity
    _hop_distance = this->Determine_Hopping_Distance();
    
    // Break periodicity
    this->Break_periodicity(true, false, false);
    
    // Translate the graph due to the spatial location of the electrodes and update system box size accordingly, putting the left electrode at x = 0
    // left_electrode_distance is the distance of the left electrode to the node with minimum x-coordinate
    
    votca::tools::vec pos = this->_nodes[0]->position(); // initial value 
    double minX = pos.x();

    typename std::vector<TNode*>::iterator it;      
    for(it = this->_nodes.begin(); it != this->_nodes.end(); it++) {
        pos = (*it)->position(); if(pos.x() < minX) {minX = pos.x();}
    }

   //distance by which the graph should be translated is left_electrode_distance - minX
 
    double xtranslate = left_distance - minX;

    for(it = this->_nodes.begin(); it != this->_nodes.end(); it++) {
        votca::tools::vec oldpos = (*it)->position();
        double newxpos = oldpos.x() + xtranslate;
        votca::tools::vec newpos = votca::tools::vec(newxpos,oldpos.y(),oldpos.z());
        (*it)->SetPosition(newpos);
    }

    //determine system box size and adjust accordingly to given electrode distances

    votca::tools::vec old_sim_box_size = this->Determine_Sim_Box_Size();
    double new_sim_box_sizeX = old_sim_box_size.x() + left_distance + right_distance;
    _sim_box_size =  votca::tools::vec(new_sim_box_sizeX, old_sim_box_size.y(), old_sim_box_size.z());
    
    //set node types for existing nodes as Normal
    for(it = this->_nodes.begin(); it != this->_nodes.end(); it++) (*it)->SetType((int) Normal);
    
    //introduce the electrode nodes (might make a special electrode node header file for this)
    _left_electrode = new TNode(-1, tools::vec(0.0,0.0,0.0));
    _right_electrode = new TNode(-2, tools::vec(_sim_box_size.x(),0.0,0.0));
    _left_electrode->SetType((int) LeftElectrode);
    _right_electrode->SetType((int) RightElectrode);

    //determine the nodes which are injectable from the left electrode and the nodes which are injectable from the right electrode

    for(it  = this->_nodes.begin(); it != this->_nodes.end(); it++) { 
      
        votca::tools::vec nodepos = (*it)->position();
        double left_distance = nodepos.x();
        int linkID = this->_links.size();
     
        if(left_distance <= _hop_distance) {
            votca::tools::vec dr = votca::tools::vec(-1.0*left_distance,0.0,0.0);   
            TLink* newLinkCollect = this->AddLink(linkID,(*it), _left_electrode, dr); linkID++;
            TLink* newLinkInject = new TLink(linkID, _left_electrode, (*it), -1.0*dr);
            _left_electrode->AddLink(newLinkInject);
        }
      
        double right_distance = _sim_box_size.x() - nodepos.x();
        if(right_distance <= _hop_distance) {
            votca::tools::vec dr = votca::tools::vec(right_distance,0.0,0.0);   
            TLink* newLinkCollect = this->AddLink(linkID,(*it), _right_electrode, dr); linkID++;
            TLink* newLinkInject = new TLink(linkID, _right_electrode, (*it), -1.0*dr);
            _right_electrode->AddLink(newLinkInject);
        }
    }

    // associate links in links vector with the corresponding nodes
    this->LinkSort();
    
    // determine maximum degree of graph
    _max_pair_degree = this->Determine_Max_Pair_Degree();

}


template<class TGraph, class TNode, class TLink>
void GraphDevice<TGraph, TNode, TLink>::LinkSort(){
    
    typename std::vector<TLink*>::iterator it;
    for (it = this->_links.begin(); it != this->_links.end(); it++ ) {
        TNode* node1 = dynamic_cast<TNode*>((*it)->node1());
        node1->AddLink((*it));
    }
}

template<class TGraph, class TNode, class TLink>
int GraphDevice<TGraph, TNode, TLink>::Determine_Max_Pair_Degree(){
    
    int max_pair_degree = 0;
    typename std::vector<TNode*>::iterator it;    
    for(it = this->_nodes.begin(); it != this->_nodes.end(); it++) {
        if((*it)->links().size()>max_pair_degree) max_pair_degree = (*it)->links().size();
    }
    return max_pair_degree;
    
}

template<class TGraph, class TNode, class TLink>
double GraphDevice<TGraph, TNode, TLink>::Determine_Hopping_Distance(){
    
    double hop_distance = 0.0;
    typename std::vector<TLink*>::iterator it;    
    for(it = this->_links.begin(); it != this->_links.end(); it++) {
        votca::tools::vec dR = (*it)->r12();
        double distance = abs(dR);
        if(distance>hop_distance) hop_distance = distance;
    }
    return hop_distance;
}


template<class TGraph, class TNode, class TLink>
votca::tools::vec GraphDevice<TGraph, TNode, TLink>::Determine_Sim_Box_Size(){ 
    
    votca::tools::vec sim_box_size;
    //Determination of simulation box size
    //Note that it is possible that none of the pairs pass the simulation box boundaries
    //In this special case, we must determine the node with max x/y/z coordinate and min x/y/z coordinate
    
    //is a boundary crossing pair being found?
    bool pairXfound = false; bool pairYfound = false; bool pairZfound = false;
    
    //for determination of initial value of simboxsize
    bool initXfound = false; bool initYfound = false; bool initZfound = false;
    double newX; double newY; double newZ;
    
    //dimensions of the simulation box
    double simX; double simY; double simZ;
    
    //initial values for maximum and minimum coordinates
    votca::tools::vec initpos = this->_nodes[0]->position();
    double maxX = initpos.x(); double maxY = initpos.y(); double maxZ = initpos.z();
    double minX = initpos.x(); double minY = initpos.y(); double minZ = initpos.z();
    
    typename std::vector<TLink*>::iterator it;    
    for(it = this->_links.begin(); it != this->_links.end(); it++) {
        
        if(pairXfound&&pairYfound&&pairZfound) break;
        
        votca::tools::vec pos1 = (*it)->node1()->position();
        votca::tools::vec pos2 = (*it)->node2()->position();
        votca::tools::vec dr = (*it)->r12();

        if(maxX<pos1.x()) {maxX = pos1.x();}   if(minX>pos1.x()) {minX = pos1.x();}
        if(maxY<pos1.y()) {maxY = pos1.y();}   if(minY>pos1.y()) {minY = pos1.y();}
        if(maxZ<pos1.z()) {maxZ = pos1.z();}   if(minZ>pos1.z()) {minZ = pos1.z();}

        //theoretical possibility that hopping distance is larger than the actual simulation box size (pair crosses boundaries more than once), in which the value of simboxsize is to large
        //check for minimum to catch these exceptional cases
        
        if(pos2.x()-pos1.x() < 0.0  && dr.x() > 0.0 ) { pairXfound = true; newX = pos1.x() + dr.x() - pos2.x(); if(!initXfound) {initXfound = true; simX = newX;} else if(simX>newX) { simX = newX;} }
        if(pos2.x()-pos1.x() > 0.0  && dr.x() < 0.0 ) { pairXfound = true; newX = pos2.x() - dr.x() - pos1.x(); if(!initXfound) {initXfound = true; simX = newX;} else if(simX>newX) { simX = newX;} }
        if(pos2.y()-pos1.y() < 0.0  && dr.y() > 0.0 ) { pairYfound = true; newY = pos1.y() + dr.y() - pos2.y(); if(!initYfound) {initYfound = true; simY = newY;} else if(simY>newY) { simY = newY;} }
        if(pos2.y()-pos1.y() > 0.0  && dr.y() < 0.0 ) { pairYfound = true; newY = pos2.y() - dr.y() - pos1.y(); if(!initYfound) {initYfound = true; simY = newY;} else if(simY>newY) { simY = newY;} }
        if(pos2.z()-pos1.z() < 0.0  && dr.z() > 0.0 ) { pairZfound = true; newZ = pos1.z() + dr.z() - pos2.z(); if(!initZfound) {initZfound = true; simZ = newZ;} else if(simZ>newZ) { simZ = newZ;} }
        if(pos2.z()-pos1.z() > 0.0  && dr.z() < 0.0 ) { pairZfound = true; newZ = pos2.z() - dr.z() - pos1.z(); if(!initZfound) {initZfound = true; simZ = newZ;} else if(simZ>newZ) { simZ = newZ;} }
        
    }
    
    //for the possible outcome that none of the pairs have crossed the simulation box boundary
    if(!pairXfound) {simX = maxX-minX;}
    if(!pairYfound) {simY = maxY-minY;}
    if(!pairZfound) {simZ = maxZ-minZ;}

    sim_box_size = votca::tools::vec(simX,simY,simZ);
    return sim_box_size;
}

template<class TGraph, class TNode, class TLink>
void GraphDevice<TGraph, TNode, TLink>::Break_periodicity(bool break_x, bool break_y, bool break_z){

    for(int it = this->_links.size()-1; it != 0; it--) {

        TLink* ilink = this->_links[it];
        
        votca::tools::vec pos1 = ilink->node1()->position();
        votca::tools::vec pos2 = ilink->node2()->position();
        votca::tools::vec dr = ilink->r12();

        bool remove_flag = false;
        
        if(break_x){ if(pos2.x()-pos1.x() < 0.0  && dr.x() > 0.0 ) remove_flag = true;  if(pos2.x()-pos1.x() > 0.0  && dr.x() < 0.0 ) remove_flag = true; }        
        if(break_y){ if(pos2.y()-pos1.y() < 0.0  && dr.y() > 0.0 ) remove_flag = true;  if(pos2.y()-pos1.y() > 0.0  && dr.y() < 0.0 ) remove_flag = true; }
        if(break_z){ if(pos2.z()-pos1.z() < 0.0  && dr.z() > 0.0 ) remove_flag = true;  if(pos2.z()-pos1.z() > 0.0  && dr.z() < 0.0 ) remove_flag = true; }
        
        if(remove_flag) this->RemoveLink(it);
        
    }
    
    //renumber link id's (need to check whether this is really necessary)
    this->RenumberId();
}

template<class TGraph, class TNode, class TLink>
void GraphDevice<TGraph,TNode, TLink>::RenumberId() {
    int resetID = 0;
    typename std::vector<TLink*>::iterator it;
    for (it = this->_links.begin(); it != this->_links.end(); it++) {
        (*it)->SetID(resetID);
        resetID++;
    }
    // are pair IDs necessary?
}
    
}}

/*this->_links.erase(link_id-id_shift)*/

#endif
