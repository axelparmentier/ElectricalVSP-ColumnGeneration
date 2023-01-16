//
//  CostConstraintConditionalBoundMeetAlgorithmFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__CostConstraintConditionalBoundMeetAlgorithmFunctor__
#define __latticeRCSP_v2__CostConstraintConditionalBoundMeetAlgorithmFunctor__

#include <stdio.h>
#include "../GraphWithConditionalBounds.h"
//#include "../ConditionalGraph.h"


namespace latticeRCSP {

    template <typename Resource, typename ConditionalFunctor, typename Problem>
    class CostConstraintConditionalBoundMeetAlgorithmFunctor {
        
        GraphWithConditionalBounds<Resource, ConditionalFunctor, Problem> * graph_m;
        vector<int> vertexOfState;
        
    public:
        CostConstraintConditionalBoundMeetAlgorithmFunctor(GraphWithConditionalBounds<Resource, ConditionalFunctor, Problem> *, vector<int>);
        bool getKey(int vertex, Resource &) const;
    };
}

#include "CostConstraintConditionalBoundMeetAlgorithmFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__CostConstraintConditionalBoundMeetAlgorithmFunctor__) */
