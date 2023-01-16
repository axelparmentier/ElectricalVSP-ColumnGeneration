//
//  ClusterFunctors.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ClusterFunctors__
#define __latticeRCSP_v2__ClusterFunctors__

#include <stdio.h>
#include "ClusterContainer.h"


namespace latticeClustering {
    
    template<typename Resource>
    class SumOfDistancesToMeetFunctor {
    public:
        static double getCost(ClusterContainer<Resource> &);
    };
    
    template<typename Resource, typename AlternativeDistanceToMeetFunctor>
    class SumOfAlternativeDistanceToMeetFunctor {
    public:
        static double getCost(ClusterContainer<Resource> &);
    };

    template<typename Resource, typename AlternativeDistanceToMeetFunctor>
    class WeightedSumOfAlternativeDistanceToMeetFunctor {
    public:
        static double getCost(ClusterContainer<Resource> &);
    };
    
    template<typename Resource, typename HeuristicFunction>
    class MaxOfHeuristicFunctionOnMeetFunctor {
    public:
        static double getCost(ClusterContainer<Resource> &);
    };
    
}
#include "ClusterFunctors.hpp"

#endif /* defined(__latticeRCSP_v2__ClusterFunctors__) */
