//
//  ClusterFunctors.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ClusterFunctors.h"

namespace latticeClustering {
    
    template<typename Resource>
    double SumOfDistancesToMeetFunctor<Resource>::getCost(ClusterContainer<Resource> & clust){
        return clust.getSumOfDistancesToMeet();
    }
    
    template<typename Resource, typename AlternativeDistanceToMeetFunctor>
    double SumOfAlternativeDistanceToMeetFunctor<Resource, AlternativeDistanceToMeetFunctor>::getCost(ClusterContainer<Resource> & clust){
        
        double result = 0;
        Resource meet = clust.getMeet();
        
        typename vector<Resource const *>::const_iterator iterFixed;
        for (iterFixed = clust.getFixedElementsBegin(); iterFixed != clust.getFixedElementsEnd(); ++iterFixed) {
            result += AlternativeDistanceToMeetFunctor::getDistance(*(*iterFixed),meet);
        }
        
        typename map<int, Resource const *>::const_iterator iterNew;
        
        for (iterNew = clust.getNewElementsBegin(); iterNew != clust.getNewElementsEnd(); ++iterNew){
            result += AlternativeDistanceToMeetFunctor::getDistance(*(iterNew->second),meet);
        }
        return result;
    }


    template<typename Resource, typename AlternativeDistanceToMeetFunctor>
    double WeightedSumOfAlternativeDistanceToMeetFunctor<Resource, AlternativeDistanceToMeetFunctor>::getCost(ClusterContainer<Resource> & clust){
        
        double result = 0;
        Resource meet = clust.getMeet();
        
        typename vector<Resource const *>::const_iterator iterFixed;
        for (iterFixed = clust.getFixedElementsBegin(); iterFixed != clust.getFixedElementsEnd(); ++iterFixed) {
            result +=  AlternativeDistanceToMeetFunctor::getDistance(*(*iterFixed),meet);
        }
        
        typename map<int, Resource const *>::const_iterator iterNew;
        
        for (iterNew = clust.getNewElementsBegin(); iterNew != clust.getNewElementsEnd(); ++iterNew){
            result += AlternativeDistanceToMeetFunctor::getDistance(*(iterNew->second),meet);
        }
        
        double weight = pow(meet.expectation()*1e-3,3);
        return result/(double) weight;
        // return clust.size_nonFixedElements();
    }
    
    template<typename Resource, typename HeuristicFunction>
    double MaxOfHeuristicFunctionOnMeetFunctor<Resource,HeuristicFunction>::getCost(ClusterContainer<Resource> & clust){
        return HeuristicFunction::getKey(clust.getMeet());
    }
    
    
}
