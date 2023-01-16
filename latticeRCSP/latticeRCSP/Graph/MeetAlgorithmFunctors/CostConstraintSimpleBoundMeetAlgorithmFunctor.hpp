//
//  CostConstraintSimpleBoundMeetAlgorithmFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "CostConstraintSimpleBoundMeetAlgorithmFunctor.h"

namespace latticeRCSP {
    template <typename Resource, typename Problem>
    CostConstraintSimpleBoundMeetAlgorithmFunctor< Resource, Problem>::CostConstraintSimpleBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> * graph0){
        graph_m = graph0;
    }
    
    template <typename Resource, typename Problem>
    CostConstraintSimpleBoundMeetAlgorithmFunctor< Resource, Problem>::isDominated(int vertex, Resource & res){
        return graph_m->isDominated(vertex,res);
    }
}