//
//  CostConstraintStateGraphBoundMeetAlgorithmFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "CostConstraintStateGraphBoundMeetAlgorithmFunctor.h"

namespace latticeRCSP {
    
    template <typename Resource, typename Problem>
    CostConstraintStateGraphBoundMeetAlgorithmFunctor<Resource,Problem>::CostConstraintStateGraphBoundMeetAlgorithmFunctor(GraphWithStateGraphBounds<Resource, Problem> * pGraph, vector<int> const & vertexOfState0){
        graph_m = pGraph;
        vertexOfState = vertexOfState0;
    }
    
    template <typename Resource, typename Problem>
    bool CostConstraintStateGraphBoundMeetAlgorithmFunctor<Resource,Problem>::getKey(int vertex, Resource & r) const{
        return graph_m->isDominated(vertexOfState[vertex],r);
    }
    
    
}
