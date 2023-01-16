//
//  Cluster.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__Cluster__
#define __latticeRCSP_v2__Cluster__

#include <stdio.h>
#include "ClusterContainer.h"

namespace latticeClustering {
    
    template <typename Resource, typename ClusterCostFunctor>
    class Cluster{
        ClusterContainer<Resource> container;
        double cost;
        
    public:
        Cluster();
        Cluster(vector<Resource const *> const &);
        
        unsigned int size_allElements() const;
        unsigned int size_nonFixedElements() const;
        
        double insertElement(int, const Resource *);
        double removeElement(int);
//        double swap(int elementRemoved,int elementAdded, const Resource * elAdded);
        void clear();
        
        double getCost() const;
        Resource getJoin() ;
        Resource getMeet() ;
        
        vector<int> getNewElements() const;
    };
}

#include "Cluster.hpp"
#endif /* defined(__latticeRCSP_v2__Cluster__) */
