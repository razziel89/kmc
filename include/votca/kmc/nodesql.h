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
int id, double posX, double posY, double posZ, 
            double UnCnNe, double UnCnNh, double UcNcCe, double UcNcCh, 
            double eAnion, double eNeutral, double eCation, 
            double ucCnNe, double ucCnNh) : 
          _id(id), 
          _UnCnNe(UnCnNe), _UnCnNh(UnCnNh), _UcNcCh(UcNcCh),
          _eAnion(eAnion), _eNeutral(eNeutral), _eCation(eCation
    setU(double UnCnNe, double UnCnNh, double UcNcCe, double UcNcCh){
             ; // FILL IN
    }
         
    setE(double eAnion, double eNeutral, double eCation) {
        ;
    }

         
    
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

    
};


}}

#endif	/* _VOTCA_KMC_NODE_H */
