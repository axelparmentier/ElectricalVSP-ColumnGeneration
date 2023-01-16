//
//  ToolsForLatticeRCSP.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 12/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ToolsForLatticeRCSP__
#define __latticeRCSP_v2__ToolsForLatticeRCSP__

#include <stdio.h>
#include <vector>
#include <map>

using namespace std;

namespace latticeRCSP {
    class ToolsForLatticeRCSP{
    public:
        template <typename elementsType>
        static void deleteElementsOfPointersVector(vector<elementsType *> &);
        
        template <typename keyType, typename elementsType>
        static vector<elementsType> convertMapIntoVector(map<keyType, elementsType> const &);
    };
}

#include "ToolsForLatticeRCSP.hpp"

#endif /* defined(__latticeRCSP_v2__ToolsForLatticeRCSP__) */
