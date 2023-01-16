//
//  ToolsForLatticeRCSP.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 12/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ToolsForLatticeRCSP.h"

namespace latticeRCSP {
    template<typename elementsType>
    void ToolsForLatticeRCSP::deleteElementsOfPointersVector(vector<elementsType *> & vect){
        for (int i = 0; i< vect.size(); i++) {
            delete vect[i];
        }
    }
    
    template<typename keyType, typename elementsType>
    vector<elementsType> ToolsForLatticeRCSP::convertMapIntoVector(const map<keyType, elementsType> & map0){
        vector<elementsType> solution(map0.size());
        int i =0;
        for (typename map<keyType, elementsType>::iterator iter = map0.begin(); iter != map0.end(); ++iter) {
            solution[i] = iter->second;
            i++;
        }
    }
}
