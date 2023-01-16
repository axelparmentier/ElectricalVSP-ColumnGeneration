//
//  CostConstraintStateGraphBoundMeetAlgorithmFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__CostConstraintStateGraphBoundMeetAlgorithmFunctor__
#define __latticeRCSP_v2__CostConstraintStateGraphBoundMeetAlgorithmFunctor__

#include <stdio.h>
#include "../GraphWithStateGraphBounds.h"

namespace latticeRCSP {
    
    template <typename Resource, typename Problem>
    class CostConstraintStateGraphBoundMeetAlgorithmFunctor {
        
        GraphWithStateGraphBounds<Resource, Problem> * graph_m;
        vector<int> vertexOfState;
        
    public:
        CostConstraintStateGraphBoundMeetAlgorithmFunctor(GraphWithStateGraphBounds<Resource, Problem> *, vector<int> const &);
        bool getKey(int vertex, Resource &) const;
    };
}

#include "CostConstraintStateGraphBoundMeetAlgorithmFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__CostConstraintStateGraphBoundMeetAlgorithmFunctor__) */
