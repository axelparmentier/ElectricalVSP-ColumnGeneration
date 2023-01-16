//
//  CostConstraintSimpleBoundMeetAlgorithmFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__CostConstraintSimpleBoundMeetAlgorithmFunctor__
#define __latticeRCSP_v2__CostConstraintSimpleBoundMeetAlgorithmFunctor__

#include <stdio.h>
#include "../GraphWithBounds.h"



namespace latticeRCSP {
    template <typename Resource, typename Problem>
    class CostConstraintSimpleBoundMeetAlgorithmFunctor {
        
        GraphWithBounds<Resource, Problem> * graph_m;
        
    public:
        CostConstraintSimpleBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> *);
        bool isDominated(int vertex, Resource &) const;
    };
}

#include "CostConstraintSimpleBoundMeetAlgorithmFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__CostConstraintSimpleBoundMeetAlgorithmFunctor__) */
