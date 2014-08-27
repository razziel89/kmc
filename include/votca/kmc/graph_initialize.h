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

#ifndef __VOTCA_KMC_GRAPH_INITIALIZE_H_
#define __VOTCA_KMC_GRAPH_INITIALIZE_H_

#include <vector>
#include <fstream>
#include <iostream>
#include <votca/kmc/graph.h>
#include <votca/kmc/eventinfo.h>
#include <votca/tools/database.h>
#include <votca/tools/random2.h>


namespace votca { namespace kmc {

template<class TNode, class TLink>    
class Graph_Initialize : public Graph<TNode, TLink> {

public:
   
    /// Reads node information from filename
    void Initialize_sql(string filename);
    
    /// Create a cubic morphology
    void Initialize_cubic(Eventinfo* eventinfo, votca::tools::Random2 *RandomVariable);
    
};

template<class TNode, class TLink>    
inline void Graph_Initialize<TNode,TLink>::Initialize_sql(string filename)
{   
    // Load Nodes
    votca::tools::Database db;
    db.Open( filename );
    votca::tools::Statement *stmt = db.Prepare("SELECT _id-1, posX, posY, posZ, UnCnNe, UnCnNh, UcNcCe, UcNcCh, eAnion, eNeutral, eCation, UcCnNe, UcCnNh FROM segments;");

    while (stmt->Step() != SQLITE_DONE) 
    {    
        int id = stmt->Column<double>(0);
        double posX = stmt->Column<double>(1);
        double posY = stmt->Column<double>(2);
        double posZ = stmt->Column<double>(3);
        votca::tools::vec position(posX,posY,posZ);

        double UnCnNe = stmt->Column<double>(4);
        double UnCnNh = stmt->Column<double>(5);
        double UcNcCe = stmt->Column<double>(6);
        double UcNcCh = stmt->Column<double>(7);
        double eAnion = stmt->Column<double>(8);
        double eNeutral = stmt->Column<double>(9);
        double eCation = stmt->Column<double>(10);

        double UcCnNe = stmt->Column<double>(11);
        double UcCnNh = stmt->Column<double>(12);

        TNode* newTNode = this->AddNode(id,position);
        newTNode->setU(UnCnNe, UnCnNh, UcNcCe, UcNcCh);
        newTNode->setE(eAnion, eNeutral, eCation);
        newTNode->setu(UcCnNe, UcCnNh);
    }
    
    std::cout << "nodes read" << endl;
    
    delete stmt;

    // Load Node Pairs

    stmt = db.Prepare("SELECT seg1-1 AS 'segment1', seg2-1 AS 'segment2', drX, drY, drZ, rate12e, rate12h, rate21e, rate21h, Jeff2e, Jeff2h, lOe, lOh  FROM pairs UNION SELECT seg2-1 AS 'segment1', seg1-1 AS 'segment2', -drX AS 'drX', -drY AS 'drY', -drZ AS 'drZ', rate21e AS 'rate12e', rate21h AS 'rate12h', rate12e AS 'rate21e', rate12h AS 'rate21h',Jeff2e, Jeff2h, lOe, lOh  FROM pairs ORDER BY segment1;");
    
    long id = 0;
    while (stmt->Step() != SQLITE_DONE) 
    {
        int node1_id = stmt->Column<int>(0);
        int node2_id = stmt->Column<int>(1);
        TNode* node1 = this->GetNode(node1_id);
        TNode* node2 = this->GetNode(node2_id);

        double drX = stmt->Column<double>(2);
        double drY = stmt->Column<double>(3);
        double drZ = stmt->Column<double>(4);
        votca::tools::vec r12(drX,drY,drZ);
        
        double rate12e = stmt->Column<double>(5);
        double rate12h = stmt->Column<double>(6);
        double rate21e = stmt->Column<double>(7);
        double rate21h = stmt->Column<double>(8);
        
        double Jeff2e = stmt->Column<double>(9);
        double Jeff2h = stmt->Column<double>(10);
        
        double lOe = stmt->Column<double>(11);
        double lOh = stmt->Column<double>(12);
        
        TLink* newTLink = this->AddLink(id,node1, node2, r12);
        newTLink->setRate(rate12e,rate12h,rate21e,rate21h);
        newTLink->setJeff2(Jeff2e,Jeff2h);
        newTLink->setlO(lOe,lOh);
        id++;
    }
        
    std::cout << "links read" << endl;
        
    delete stmt;
    stmt = NULL;
}

template<class TNode, class TLink> 
inline void Graph_Initialize<TNode,TLink>::Initialize_cubic(Eventinfo* eventinfo, votca::tools::Random2 *RandomVariable) 
{
    int NX = eventinfo->NX;
    int NY = eventinfo->NY;
    int NZ = eventinfo->NZ;
    
    int node_index = 0;
    
    for(int ix=0; ix<NX; ix++) {
        for(int iy=0; iy<NY; iy++) {
            for(int iz=0; iz<NZ; iz++) {
                
                int id = node_index;
                votca::tools::vec position = votca::tools::vec(ix*lat_const,iy*lat_const,iz*lat_const);
                TNode* newTNode = this->AddNode(id,position);

                node_index++;    
                
                double randn_el = RandomVariable->rand_gaussian(eventinfo->el_disorder);
                double UcCnNe = randn_el; //disorder
                double randn_ho;
                double UcCnNh;
                if(eventinfo->el_ho_correlation == 0) { //no correlation
                    randn_ho = RandomVariable->rand_gaussian(eventinfo->ho_disorder);
                }
                else if(eventinfo->el_ho_correlation == 1) { //perfect correlation
                    randn_ho = 1.0*randn_el;
                }
                else if(eventinfo->el_ho_correlation == -1) { // perfect anti-correlation
                    randn_ho = -1.0*randn_el;
                }
                UcCnNh = randn_ho;
                
                double eAnion = eventinfo->lumo; //lumo
                double eNeutral = 0.0;
                double eCation = eventinfo->homo; //homo

                newTNode->setE(eAnion, eNeutral, eCation);
                newTNode->setu(UcCnNe, UcCnNh);                
                
            }
        }
    }
    
    node_index = 0;
    int hopd = ceil(eventinfo->hop_distance);
    long link_index = 0;                      
    
    for(int ix=0; ix<NX; ix++) {
        for(int iy=0; iy<NY; iy++) {
            for(int iz=0; iz<NZ; iz++) {
                
                int node1_id = node_index;
                TNode* node1 = this->GetNode(node1_id);
                for(int dx = - hopd; dx <= hopd; dx++) {
                    for(int dy = - hopd; dy <= hopd; dy++) {
                        for(int dz = - hopd; dz <= hopd; dz++) {
                            if(!(dx == 0 && (dy == 0 && dz == 0))) {
                
                                int new_ix = ix + dx;
                                int new_iy = iy + dy;
                                int new_iz = iz + dz;

                                if(new_ix<0) new_ix += NX;
                                if(new_ix>NX-1) new_ix -= NX;
                                if(new_iy<0) new_iy += NY;
                                if(new_iy>NY-1) new_iy -= NY;
                                if(new_iz<0) new_iz += NZ;
                                if(new_iz>NZ-1) new_iz -= NZ;

                                int node2_id = new_iz + new_iy*NZ + new_ix*NZ*NY;
                                TNode* node2 = this->GetNode(node2_id);
                                
                                double drX = 1.0*dx;
                                double drY = 1.0*dy;
                                double drZ = 1.0*dz;
                                votca::tools::vec r12(drX,drY,drZ);                               
                                TLink* newTLink = this->AddLink(link_index,node1, node2, r12);
                                link_index++;
                            }
                        }
                    }
                }
                
                node_index++;
            }
        }
    }    
    
    int node1_id = stmt->Column<int>(0);
    int node2_id = stmt->Column<int>(1);
    TNode* node1 = this->GetNode(node1_id);
    TNode* node2 = this->GetNode(node2_id);

    double drX = stmt->Column<double>(2);
    double drY = stmt->Column<double>(3);
    double drZ = stmt->Column<double>(4);
    votca::tools::vec r12(drX,drY,drZ);
        
    TLink* newTLink = this->AddLink(id,node1, node2, r12);
    link_id++;
}

}}



#endif
