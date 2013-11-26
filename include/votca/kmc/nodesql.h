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

#ifndef _VOTCA_KMC_NODESQL_H
#define	_VOTCA_KMC_NODESQL_H

#include <votca/kmc/linksql.h>
#include <votca/kmc/node.h>

namespace votca { namespace kmc {

class NodeSQL : public Node
{
public:
    
    NodeSQL(int id, tools::vec position) : Node(id, position){
    };

//    void AddLink( LinkSQL* link ) { _links.push_back(link); }
    
    void setU(double UnCnNe, double UnCnNh, double UcNcCe, double UcNcCh){
        _UnCnNe  = UnCnNe;
        _UnCnNh  = UnCnNh;
        _UcNcCe  = UcNcCe;
        _UcNcCh  = UcNcCh;
    }
         
    void setE(double eAnion, double eNeutral, double eCation) {
        _eAnion   = eAnion;
        _eNeutral = eNeutral;
        _eCation  = eCation;
    }
    
    void setu(double ucCnNe, double ucCnNh) {
        _ucCnNe = ucCnNe;
        _ucCnNh = ucCnNh;
    }

    //Debugging
    const double &UnCnNe() const { return _UnCnNe; }     
    const double &UnCnNh() const { return _UnCnNh; }   
    const double &UcNcCe() const { return _UcNcCe; }   
    const double &UcNcCh() const { return _UcNcCh; }   

    const double &eAnion() const { return _eAnion; }       
    const double &eNeutral() const { return _eNeutral; }   
    const double &eCation() const { return _eCation; }   

    const double &ucCnNe() const { return _ucCnNe; }   
    const double &ucCnNh() const { return _ucCnNh; }   
    
private:

    double _UnCnNe; 
    double _UnCnNh; 
    double _UcNcCe; 
    double _UcNcCh; 

    double _eAnion;
    double _eNeutral;
    double _eCation;

    double _ucCnNe;
    double _ucCnNh;

    ;
    
};


}}

#endif	/* _VOTCA_KMC_NODESQL_H */

