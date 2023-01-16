//
//  CostConstraintConditionalBoundMeetAlgorithmFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 10/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "CostConstraintConditionalBoundMeetAlgorithmFunctor.h"

namespace latticeRCSP {
    template <typename Resource, typename ConditionalFunctor, typename Problem>
    CostConstraintConditionalBoundMeetAlgorithmFunctor< Resource, ConditionalFunctor, Problem>::CostConstraintConditionalBoundMeetAlgorithmFunctor(GraphWithConditionalBounds<Resource, ConditionalFunctor, Problem> * graph0, vector<int> vertexOfState0){
        cout << "CostConstraintConditionalBoundMeetAlgorithmFunctor is used" << endl;
        graph_m = graph0;
        vertexOfState = vertexOfState0;
    }
    
    template <typename Resource, typename ConditionalFunctor, typename Problem>
    bool CostConstraintConditionalBoundMeetAlgorithmFunctor< Resource, ConditionalFunctor, Problem>::getKey(int vertex, Resource & res) const{
        return graph_m->isDominated(vertexOfState[vertex],res);
    }
}