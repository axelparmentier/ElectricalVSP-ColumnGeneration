//
//  ClusterResourceNSample.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 23/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ClusterResourceNSample__
#define __latticeRCSP_v2__ClusterResourceNSample__

#include <stdio.h>
#include "ResourceNSample.h"
#include "../../Clustering/ClusterContainer.h"

using namespace latticeClustering;

namespace ResourceSample {
    class ClusterResourceNSample : public ClusterContainer<ResourceNSample>{
        double cost;
        
    public:
        ClusterResourceNSample();
        ClusterResourceNSample(vector<ResourceNSample const *> const &);
        
        void computeCost();
        
        double insertElement(int, const ResourceNSample *);
        double removeElement(int);
        //        double swap(int elementRemoved,int elementAdded, const Resource * elAdded);
        void clear();
        double getCost() const;
    };
}


#endif /* defined(__latticeRCSP_v2__ClusterResourceNSample__) */
