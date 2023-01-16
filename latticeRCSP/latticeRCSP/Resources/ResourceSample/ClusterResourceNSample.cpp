//
//  ClusterResourceNSample.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 23/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ClusterResourceNSample.h"


namespace ResourceSample {
    ClusterResourceNSample::ClusterResourceNSample() : ClusterContainer<ResourceNSample>() {
        cost = 0;
    }
    
    ClusterResourceNSample::ClusterResourceNSample(vector<ResourceNSample const *> const & clus) : ClusterContainer<ResourceSample::ResourceNSample>(clus){
        computeCost();
    }
    
    double ClusterResourceNSample::insertElement(int e, const ResourceNSample * elRes){
        double oldCost = cost;
        ClusterContainer<ResourceNSample>::insertElement(e,elRes);
        computeCost();
        return cost - oldCost;
    }
    
    double ClusterResourceNSample::removeElement(int e){
        double oldCost = cost;
        ClusterContainer<ResourceNSample>::removeElement(e);
        computeCost();
        return cost - oldCost;
    }
    
    void ClusterResourceNSample::clear(){
        ClusterContainer<ResourceNSample>::clear();
        cost = 0;
    }

    double ClusterResourceNSample::getCost() const{
        return cost;
    }
    
    void ClusterResourceNSample::computeCost(){
        cost = 0;
        for (int e = 0; e < fixedElements.size(); e++) {
            cost += fixedElements[e]->weightedDistance(meet);
        }
        for (map<int,ResourceNSample const *>::const_iterator it = newElements.begin() ; it != newElements.end(); ++it) {
            cost += it->second->weightedDistance(meet);
        }
    }


}